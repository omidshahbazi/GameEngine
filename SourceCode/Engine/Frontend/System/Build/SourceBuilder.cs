// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using System;
using System.IO;

namespace Engine.Frontend.System.Build
{
	class SourceBuilder
	{
		public enum States
		{
			NotBuilt = 0,
			Built,
			Failed
		}

		private const string BinariesFolderName = "Binaries";

		private string sourcePathRoot = "";

		private States state = States.NotBuilt;

		private string intermediateModulePath = "";
		private string generatedFilesPath = "";
		private Compiler compiler = new Compiler();
		private static CommandLineProcess reflectionGeneratorProcess = null;

		private string BinariesPath
		{
			get { return intermediateModulePath + BinariesFolderName + EnvironmentHelper.PathSeparator; }
		}

		public BuildRules BuildRule
		{
			get;
			private set;
		}

		public BuildRules.RuleBase SelectedRule
		{
			get;
			private set;
		}

		public States State
		{
			get { return state; }
			set { state = value; }
		}

		public SourceBuilder(BuildRules Rules, string SourcePathRoot)
		{
			BuildRule = Rules;

			foreach (BuildRules.RuleBase rule in BuildRule.Rules)
			{
				if (((int)rule.Configuration & (int)BuildSystem.BuildConfiguration) == (int)BuildSystem.BuildConfiguration)
				{
					SelectedRule = rule;
					break;
				}
			}

			sourcePathRoot = SourcePathRoot;

			intermediateModulePath = EnvironmentHelper.IntermediateDirectory + Rules.ModuleName + EnvironmentHelper.PathSeparator;
			generatedFilesPath = intermediateModulePath + "Generated" + EnvironmentHelper.PathSeparator;

			if (SelectedRule.LibraryUseType != BuildRules.LibraryUseTypes.UseOnly)
			{
				if (!Directory.Exists(generatedFilesPath))
					Directory.CreateDirectory(generatedFilesPath);
			}

			compiler.ErrorRaised += ErrorRaised;
		}

		public bool Build()
		{
			if (state == States.Built)
				return true;
			else if (state == States.Failed)
				return false;

			if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.UseOnly)
			{
				string[] temp = SelectedRule.BinariesPath;

				if (temp != null)
					foreach (string file in temp)
						File.Copy(sourcePathRoot + file, EnvironmentHelper.FinalOutputDirectory + Path.GetFileName(file), true);

				state = States.Built;
			}
			else if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.ProjectFile)
				BuildProjectFile();
			else
				GenerateAndBuildProjectFile();

			return (state == States.Built);
		}

		private void LogInfo()
		{
			LogHelper.DeleteLog(BuildRule.ModuleName);
			ConsoleHelper.WriteLineInfo("Building " + BuildRule.ModuleName);
		}

		private void GenerateAndBuildProjectFile()
		{
			LogInfo();

			CPPProject cppProj = new CPPProject();
			CPPProject.Profile profile = (CPPProject.Profile)cppProj.CreateProfile();

			profile.AssemblyName = SelectedRule.TargetName;
			profile.OutputType = BuildSystemHelper.LibraryUseTypesToOutputType(SelectedRule.LibraryUseType);
			profile.OutputPath = BinariesPath;
			profile.Optimization = CPPProject.Profile.Optimizations.Disabled;
			profile.MinimalRebuild = true;
			profile.PlatformType = BuildSystem.PlatformType;
			profile.BuildConfiguration = BuildSystem.BuildConfiguration;

			if (profile.BuildConfiguration == ProjectBase.ProfileBase.BuildConfigurations.Debug)
			{
				profile.GenerateDebugInformation = true;

				//if (rules.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
				//	vcproj.RuntimeLibrary = VCProjectFileGenerator.RuntimeLibraries.MultiThreadedDebug;
				//else
				profile.RuntimeLibrary = CPPProject.Profile.RuntimeLibraries.MultiThreadedDebugDLL;
			}
			else
				profile.GenerateDebugInformation = false;
			{
				//if (rules.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
				//	vcproj.RuntimeLibrary = VCProjectFileGenerator.RuntimeLibraries.MultiThreaded;
				//else
				profile.RuntimeLibrary = CPPProject.Profile.RuntimeLibraries.MultiThreadedDLL;
			}

			profile.AddIncludeDirectories(EnvironmentHelper.ProcessDirectory);
			profile.AddIncludeDirectories(sourcePathRoot);
			profile.AddIncludeDirectories(generatedFilesPath);
			if (SelectedRule.DependencyModulesName != null)
				foreach (string dep in SelectedRule.DependencyModulesName)
				{
					SourceBuilder builder = BuildSystem.GetSourceBuilder(dep);

					if (builder == null)
						continue;

					profile.AddIncludeDirectories(FileSystemUtilites.PathSeperatorCorrection(builder.sourcePathRoot + builder.SelectedRule.RootPath));

					if (builder.SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.UseOnly)
					{
						string[] temp = builder.SelectedRule.LibrariesPath;

						if (temp != null)
							foreach (string file in temp)
								profile.AddIncludeLibraries(builder.sourcePathRoot + file);
					}
					else
					{
						profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(builder.SelectedRule.TargetName, BuildSystemHelper.APIPreprocessorValues.Import));

						string[] libFiles = FileSystemUtilites.GetAllFiles(builder.BinariesPath, "*" + EnvironmentHelper.StaticLibraryExtentions);

						if (libFiles != null)
							foreach (string libFile in libFiles)
								profile.AddIncludeLibraries(libFile);
					}
				}

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(SelectedRule.TargetName, BuildSystemHelper.APIPreprocessorValues.Export));
			if (SelectedRule.PreprocessorDefinitions != null)
				foreach (string def in SelectedRule.PreprocessorDefinitions)
					profile.AddPreprocessorDefinition(def);

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetConfigurationModePreprocessor(BuildSystem.BuildConfiguration));

			if (SelectedRule.DependencyStaticLibraries != null)
				foreach (string lib in SelectedRule.DependencyStaticLibraries)
					profile.AddIncludeLibraries(lib);

			bool isThisCommonModule = (SelectedRule.TargetName == "Common");

			string[] files = FileSystemUtilites.GetAllFiles(sourcePathRoot, "*.h*");
			foreach (string file in files)
			{
				if (BuildSystem.GenerateReflection && !isThisCommonModule)
				{
					string outputBaseFileName = generatedFilesPath + Path.GetFileNameWithoutExtension(file) + ".Reflection";
					if (ParseForReflection(file, outputBaseFileName))
					{
						cppProj.AddIncludeFile(outputBaseFileName + ".h");
						cppProj.AddCompileFile(outputBaseFileName + ".cpp");
					}
				}

				cppProj.AddIncludeFile(file);
			}

			if (SelectedRule.IncludeModulesName != null)
				foreach (string moduleName in SelectedRule.IncludeModulesName)
				{
					SourceBuilder builder = BuildSystem.GetSourceBuilder(moduleName);

					if (builder == null)
						continue;

					profile.AddIncludeDirectories(builder.sourcePathRoot);
					profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(builder.SelectedRule.TargetName, BuildSystemHelper.APIPreprocessorValues.Empty));
				}

			if (SelectedRule.AdditionalIncludeDirectory != null)
				foreach (string dir in SelectedRule.AdditionalIncludeDirectory)
				{
					string dirInfo = FileSystemUtilites.PathSeperatorCorrection(dir);

					if (dirInfo.StartsWith("" + EnvironmentHelper.PathSeparator))
						dirInfo = dirInfo.Substring(1);

					profile.AddIncludeLibraryDirectories(sourcePathRoot + dirInfo);
				}

			files = FileSystemUtilites.GetAllFiles(sourcePathRoot, "*.c", "*.cpp", "*.cxx");
			foreach (string file in files)
				cppProj.AddCompileFile(file);

			profile.IntermediatePath = intermediateModulePath;

			BuildProjectFile(profile);
		}

		private void BuildProjectFile()
		{
			LogInfo();

			if (compiler.Build(generatedFilesPath, BuildSystem.BuildConfiguration, BuildSystem.PlatformType))
			{
				if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
					CopyAllFilesToFinalPath(BinariesPath, EnvironmentHelper.ExecutableExtentions);
				else if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.DynamicLibrary)
					CopyAllFilesToFinalPath(BinariesPath, EnvironmentHelper.DynamicLibraryExtentions);

				state = States.Built;

				return;
			}

			ConsoleHelper.WriteLineError("Building " + BuildRule.ModuleName + " failed");

			state = States.Failed;
		}

		private void BuildProjectFile(ProjectBase.ProfileBase ProjectProfile)
		{
			if (compiler.Build(ProjectProfile))
			{
				if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
					CopyAllFilesToFinalPath(BinariesPath, EnvironmentHelper.ExecutableExtentions);
				else if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.DynamicLibrary)
					CopyAllFilesToFinalPath(BinariesPath, EnvironmentHelper.DynamicLibraryExtentions);

				state = States.Built;

				return;
			}

			ConsoleHelper.WriteLineError("Building " + BuildRule.ModuleName + " failed");

			state = States.Failed;
		}

		private void ErrorRaised(string Text)
		{
			LogHelper.WriteLineError(BuildRule.ModuleName, Text);
			ConsoleHelper.WriteLineError(Text);
		}

		private bool ParseForReflection(string FilePath, string OutputBaseFileName)
		{
			if (FilePath.EndsWith("ReflectionDefinitions.h"))
				return false;

			if (reflectionGeneratorProcess == null)
			{
				if (!File.Exists(EnvironmentHelper.ReflectionToolPath))
					return false;

				reflectionGeneratorProcess = new CommandLineProcess();
				reflectionGeneratorProcess.FilePath = EnvironmentHelper.ReflectionToolPath;
			}

			reflectionGeneratorProcess.Start("\"" + FilePath + "\" \"" + OutputBaseFileName + "\"");

			while (!reflectionGeneratorProcess.Output.EndOfStream)
				reflectionGeneratorProcess.Output.ReadLine();

			return (reflectionGeneratorProcess.ExitCode == 0);
		}

		private static void CopyAllFilesToFinalPath(string SourcePath, string Extension)
		{
			FileSystemUtilites.CopyAllFiles(SourcePath, EnvironmentHelper.FinalOutputDirectory, true, "*" + Extension);
		}
	}
}
