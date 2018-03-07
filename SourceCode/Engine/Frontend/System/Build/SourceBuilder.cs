// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using System.Collections.Generic;
using System.IO;

namespace Engine.Frontend.System.Build
{
	class SourceBuilder
	{
		public enum States
		{
			NotBuilt = 0,
			Built,
			AlreadyUpdated,
			Failed
		}

		private const string BinariesFolderName = "Binaries";
		private const string HashesFileName = "Hash.cache";

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

		public bool Build(bool ForceToRebuild)
		{
			if (state == States.Built || state == States.AlreadyUpdated)
				return true;
			else if (state == States.Failed)
				return false;

			if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.UseOnly)
			{
				string[] temp = SelectedRule.BinariesPath;

				if (temp != null)
				{
					foreach (string file in temp)
					{
						string srcFilePath = sourcePathRoot + FileSystemUtilites.PathSeperatorCorrection(file);

						if (!File.Exists(srcFilePath))
						{
							ConsoleHelper.WriteLineError("Couldn't find file [" + srcFilePath + "]");
							state = States.Failed;
							break;
						}

						string dstFilePath = EnvironmentHelper.FinalOutputDirectory + Path.GetFileName(file);

						File.Copy(srcFilePath, dstFilePath, true);
					}
				}

				state = States.Built;
			}
			else if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.ProjectFile)
				BuildProjectFile();
			else
				GenerateAndBuildProjectFile(ForceToRebuild);

			return (state == States.Built || state == States.AlreadyUpdated);
		}

		private void LogCurrentInfo()
		{
			LogHelper.DeleteLog(BuildRule.ModuleName);
			ConsoleHelper.WriteLineInfo("Building " + BuildRule.ModuleName);
		}

		private void GenerateAndBuildProjectFile(bool ForceToRebuild)
		{
			LogCurrentInfo();

			if (!ForceToRebuild && !MustCompile())
			{
				state = States.AlreadyUpdated;
				return;
			}

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
			{
				AddAllInclusionsFromDependencies(profile, this);

				foreach (string dep in SelectedRule.DependencyModulesName)
				{
					SourceBuilder builder = BuildSystem.GetSourceBuilder(dep);

					if (builder == null)
						continue;

					profile.AddIncludeDirectories(FileSystemUtilites.GetParentDirectory(builder.sourcePathRoot));
					profile.AddIncludeDirectories(FileSystemUtilites.PathSeperatorCorrection(builder.sourcePathRoot));

					if (builder.SelectedRule.IncludesPath != null)
						foreach (string includePath in builder.SelectedRule.IncludesPath)
							profile.AddIncludeDirectories(FileSystemUtilites.PathSeperatorCorrection(builder.sourcePathRoot + includePath));

					if (builder.SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.UseOnly)
					{
						string[] temp = builder.SelectedRule.LibrariesPath;

						if (temp != null)
							foreach (string file in temp)
								profile.AddIncludeLibraries(builder.sourcePathRoot + FileSystemUtilites.PathSeperatorCorrection(file));
					}
					//else
					//{
					//	profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(builder.SelectedRule.TargetName, BuildSystemHelper.APIPreprocessorValues.Import));

					//	string[] libFiles = FileSystemUtilites.GetAllFiles(builder.BinariesPath, "*" + EnvironmentHelper.StaticLibraryExtentions);

					//	if (libFiles != null)
					//		foreach (string libFile in libFiles)
					//			profile.AddIncludeLibraries(libFile);
					//}
				}
			}

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(SelectedRule.TargetName, BuildSystemHelper.APIPreprocessorValues.Export));
			if (SelectedRule.PreprocessorDefinitions != null)
				foreach (string def in SelectedRule.PreprocessorDefinitions)
					profile.AddPreprocessorDefinition(def);

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetConfigurationModePreprocessor(BuildSystem.BuildConfiguration));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformPreprocessor(EnvironmentHelper.Platform));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformTypesPreprocessor(BuildSystem.PlatformType));

			if (SelectedRule.DependencyStaticLibraries != null)
				foreach (string lib in SelectedRule.DependencyStaticLibraries)
					profile.AddIncludeLibraries(lib);

			bool isThisCommonModule = (SelectedRule.TargetName == "Common");

			string[] files = FileSystemUtilites.GetAllFiles(sourcePathRoot, EnvironmentHelper.HeaderFileExtensions);
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

			files = FileSystemUtilites.GetAllFiles(sourcePathRoot, EnvironmentHelper.CompileFileExtensions);
			foreach (string file in files)
				cppProj.AddCompileFile(file);

			profile.IntermediatePath = intermediateModulePath;

			BuildProjectFile(profile);
		}

		private void BuildProjectFile()
		{
			LogCurrentInfo();

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

		private bool MustCompile()
		{
			VisualScriptTool.Serialization.ISerializeObject hashesData = null;

			string hashesFilePath = intermediateModulePath + HashesFileName;

			if (File.Exists(hashesFilePath))
				hashesData = VisualScriptTool.Serialization.Creator.Create<VisualScriptTool.Serialization.ISerializeObject>(File.ReadAllText(hashesFilePath));
			else
				hashesData = VisualScriptTool.Serialization.Creator.Create<VisualScriptTool.Serialization.ISerializeObject>();

			bool result = false;

			List<string> extensions = new List<string>();
			extensions.AddRange(EnvironmentHelper.HeaderFileExtensions);
			extensions.AddRange(EnvironmentHelper.CompileFileExtensions);
			string[] files = FileSystemUtilites.GetAllFiles(sourcePathRoot, extensions.ToArray());
			foreach (string file in files)
			{
				string filePathHash = GetHash(file).ToString();
				int contentHash = GetHash(File.ReadAllText(file));

				if (hashesData.Contains(filePathHash) && hashesData.Get<int>(filePathHash) == contentHash)
					continue;

				hashesData.Set(filePathHash, contentHash);

				result = true;
			}

			File.WriteAllText(hashesFilePath, hashesData.Content);

			return result;
		}

		private static void AddAllInclusionsFromDependencies(CPPProject.Profile Profile, SourceBuilder Builder)
		{
			if (Builder.SelectedRule.DependencyModulesName == null)
				return;

			foreach (string dep in Builder.SelectedRule.DependencyModulesName)
			{
				SourceBuilder builder = BuildSystem.GetSourceBuilder(dep);

				if (builder == null)
					continue;

				if (builder.SelectedRule.LibraryUseType != BuildRules.LibraryUseTypes.UseOnly)
				{
					Profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(builder.SelectedRule.TargetName, BuildSystemHelper.APIPreprocessorValues.Import));

					string[] libFiles = FileSystemUtilites.GetAllFiles(builder.BinariesPath, "*" + EnvironmentHelper.StaticLibraryExtentions);

					if (libFiles != null)
						foreach (string libFile in libFiles)
							Profile.AddIncludeLibraries(libFile);

					AddAllInclusionsFromDependencies(Profile, builder);
				}
			}
		}

		private static void CopyAllFilesToFinalPath(string SourcePath, string Extension)
		{
			FileSystemUtilites.CopyAllFiles(SourcePath, EnvironmentHelper.FinalOutputDirectory, true, "*" + Extension);
		}

		private static int GetHash(string Value)
		{
			return Value.GetHashCode();
		}
	}
}
