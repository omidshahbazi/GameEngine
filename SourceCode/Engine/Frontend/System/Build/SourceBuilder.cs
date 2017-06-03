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

		public BuildRules Rules
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
			this.Rules = Rules;
			sourcePathRoot = SourcePathRoot;

			intermediateModulePath = EnvironmentHelper.IntermediateDirectory + Rules.TargetName + EnvironmentHelper.PathSeparator;
			generatedFilesPath = intermediateModulePath + "Generated" + EnvironmentHelper.PathSeparator;

			if (this.Rules.LibraryUseType != BuildRules.LibraryUseTypes.UseOnly)
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

			if (Rules.LibraryUseType == BuildRules.LibraryUseTypes.UseOnly)
				state = States.Built;
			else if (Rules.LibraryUseType == BuildRules.LibraryUseTypes.ProjectFile)
				BuildProjectFile();
			else
				GenerateAndBuildProjectFile();

			return (state == States.Built);
		}

		private void LogInfo()
		{
			LogHelper.DeleteLog(Rules.TargetName);
			ConsoleHelper.WriteLineInfo("Building " + Rules.TargetName);
		}

		private void GenerateAndBuildProjectFile()
		{
			LogInfo();

			CPPProject cppProj = new CPPProject();
			CPPProject.Profile profile = (CPPProject.Profile)cppProj.CreateProfile();

			profile.AssemblyName = Rules.TargetName;
			profile.OutputType = BuildSystemHelper.LibraryUseTypesToOutputType(Rules.LibraryUseType);
			profile.OutputPath = BinariesPath;
			profile.Optimization = CPPProject.Profile.Optimizations.Disabled;
			profile.MinimalRebuild = true;
			profile.PlatformType = BuildSystem.PlatformType;
			profile.BuildConfiguration = BuildSystem.BuildConfiguration;

			if (profile.BuildConfiguration == CPPProject.Profile.BuildConfigurations.Debug)
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
			if (Rules.DependencyModulesName != null)
				foreach (string dep in Rules.DependencyModulesName)
				{
					SourceBuilder builder = BuildSystem.GetSourceBuilder(dep);

					if (builder == null)
						continue;

					profile.AddIncludeDirectories(builder.sourcePathRoot);

					if (builder.Rules.LibraryUseType == BuildRules.LibraryUseTypes.UseOnly)
					{
						string[] libFiles = builder.Rules.OutputsFilePath;

						if (libFiles != null)
							foreach (string libFile in libFiles)
								profile.AddIncludeLibraries(builder.sourcePathRoot + libFile);
					}
					else
					{
						profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(builder.Rules.TargetName, BuildSystemHelper.APIPreprocessorValues.Import));

						string[] libFiles = FileSystemUtilites.GetAllFiles(builder.BinariesPath, "*" + EnvironmentHelper.StaticLibraryExtentions);

						if (libFiles != null)
							foreach (string libFile in libFiles)
								profile.AddIncludeLibraries(libFile);
					}
				}

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(Rules.TargetName, BuildSystemHelper.APIPreprocessorValues.Export));
			if (Rules.PreprocessorDefinitions != null)
				foreach (string def in Rules.PreprocessorDefinitions)
					profile.AddPreprocessorDefinition(def);

			if (Rules.DependencyStaticLibraries != null)
				foreach (string lib in Rules.DependencyStaticLibraries)
					profile.AddIncludeLibraries(lib);

			bool isThisCommonModule = (Rules.TargetName == "Common");

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

			if (Rules.IncludeModulesName != null)
				foreach (string moduleName in Rules.IncludeModulesName)
				{
					SourceBuilder builder = BuildSystem.GetSourceBuilder(moduleName);

					if (builder == null)
						continue;

					profile.AddIncludeDirectories(builder.sourcePathRoot);
					profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(builder.Rules.TargetName, BuildSystemHelper.APIPreprocessorValues.Empty));
				}

			if (Rules.AdditionalCompileFile != null)
				foreach (string file in Rules.AdditionalCompileFile)
				{
					string fileInfo = FileSystemUtilites.PathSeperatorCorrection(file);
					int index = fileInfo.IndexOf(EnvironmentHelper.PathSeparator);

					SourceBuilder builder = BuildSystem.GetSourceBuilder(fileInfo.Remove(index));

					if (builder == null)
						continue;

					cppProj.AddCompileFile(builder.sourcePathRoot + fileInfo.Substring(index + 1));
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
				if (Rules.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
					CopyAllFilesToFinalPath(BinariesPath, EnvironmentHelper.ExecutableExtentions);
				else if (Rules.LibraryUseType == BuildRules.LibraryUseTypes.DynamicLibrary)
					CopyAllFilesToFinalPath(BinariesPath, EnvironmentHelper.DynamicLibraryExtentions);

				state = States.Built;

				return;
			}

			ConsoleHelper.WriteLineError("Building " + Rules.TargetName + " failed");

			state = States.Failed;
		}

		private void BuildProjectFile(ProjectBase.ProfileBase ProjectProfile)
		{
			if (compiler.Build(ProjectProfile))
			{
				if (Rules.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
					CopyAllFilesToFinalPath(BinariesPath, EnvironmentHelper.ExecutableExtentions);
				else if (Rules.LibraryUseType == BuildRules.LibraryUseTypes.DynamicLibrary)
					CopyAllFilesToFinalPath(BinariesPath, EnvironmentHelper.DynamicLibraryExtentions);

				state = States.Built;

				return;
			}

			ConsoleHelper.WriteLineError("Building " + Rules.TargetName + " failed");

			state = States.Failed;
		}

		private void ErrorRaised(string Text)
		{
			LogHelper.WriteLineError(Rules.TargetName, Text);
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
