// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using System;
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

				//if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
				//	profile.RuntimeLibrary = CPPProject.Profile.RuntimeLibraries.MultiThreadedDebug;
				//else
				profile.RuntimeLibrary = CPPProject.Profile.RuntimeLibraries.MultiThreadedDebugDLL;
			}
			else
			{
				profile.GenerateDebugInformation = false;

				//if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
				//	profile.RuntimeLibrary = CPPProject.Profile.RuntimeLibraries.MultiThreaded;
				//else
				profile.RuntimeLibrary = CPPProject.Profile.RuntimeLibraries.MultiThreadedDLL;
			}

			profile.AddIncludeDirectories(EnvironmentHelper.ProcessDirectory);
			profile.AddIncludeDirectories(sourcePathRoot);
			profile.AddIncludeDirectories(generatedFilesPath);
			if (SelectedRule.DependencyModulesName != null)
			{
				foreach (string dep in SelectedRule.DependencyModulesName)
					AddDependency(profile, dep);
			}

			if (SelectedRule.GenerateReflection)
				AddDependency(profile, BuildSystemHelper.ReflectionModuleName);

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetModuleNamePreprocessor(BuildRule.ModuleName));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(SelectedRule.TargetName, BuildSystemHelper.APIPreprocessorTypes.Export));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(SelectedRule.TargetName, BuildSystemHelper.ExternPreprocessorTypes.Fill));
			if (SelectedRule.PreprocessorDefinitions != null)
				foreach (string def in SelectedRule.PreprocessorDefinitions)
					profile.AddPreprocessorDefinition(def);

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetConfigurationModePreprocessor(BuildSystem.BuildConfiguration));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformPreprocessor(EnvironmentHelper.Platform));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformTypesPreprocessor(BuildSystem.PlatformType));

			if (SelectedRule.DependencyStaticLibraries != null)
				foreach (string lib in SelectedRule.DependencyStaticLibraries)
					profile.AddIncludeLibraries(lib);

			string[] files = FileSystemUtilites.GetAllFiles(sourcePathRoot, EnvironmentHelper.HeaderFileExtensions);
			foreach (string file in files)
			{
				cppProj.AddIncludeFile(file);

				if (SelectedRule.GenerateReflection)
				{
					string outputBaseFileName = generatedFilesPath + Path.GetFileNameWithoutExtension(file) + ".Reflection";
					if (ParseForReflection(file, outputBaseFileName))
					{
						cppProj.AddIncludeFile(outputBaseFileName + ".h");
						cppProj.AddCompileFile(outputBaseFileName + ".cpp");
					}
				}
			}

			if (SelectedRule.IncludeModulesName != null)
			{
				foreach (string moduleName in SelectedRule.IncludeModulesName)
				{
					SourceBuilder builder = BuildSystem.GetSourceBuilder(moduleName);

					if (builder == null)
						continue;

					profile.AddIncludeDirectories(builder.sourcePathRoot);
					profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(builder.SelectedRule.TargetName, BuildSystemHelper.APIPreprocessorTypes.Empty));
					profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(builder.SelectedRule.TargetName, BuildSystemHelper.ExternPreprocessorTypes.Empty));
				}
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

			if (SelectedRule.AdditionalCompileFile != null)
				foreach (string file in SelectedRule.AdditionalCompileFile)
					cppProj.AddCompileFile(EnvironmentHelper.ProcessDirectory + file);

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
			if (FilePath.EndsWith("Reflection\\Definitions.h"))
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
			if (!File.Exists(EnvironmentHelper.FinalOutputDirectory + SelectedRule.TargetName + GetExtension(this)))
				return true;

			string hashesFilePath = intermediateModulePath + HashesFileName;

			VisualScriptTool.Serialization.ISerializeObject hashesData = null;

			if (File.Exists(hashesFilePath))
				hashesData = VisualScriptTool.Serialization.Creator.Create<VisualScriptTool.Serialization.ISerializeObject>(File.ReadAllText(hashesFilePath));
			
			if (hashesData == null)
				hashesData = VisualScriptTool.Serialization.Creator.Create<VisualScriptTool.Serialization.ISerializeObject>();

			bool result = false;

			string configurationTypeName = typeof(ProjectBase.ProfileBase.BuildConfigurations).Name;
			if (!hashesData.Contains(configurationTypeName) || hashesData.Get<int>(configurationTypeName) != (int)BuildSystem.BuildConfiguration)
				result = true;
			hashesData.Set(configurationTypeName, (int)BuildSystem.BuildConfiguration);

			// TODO : Add platform type check

			if (!result)
			{
				List<string> extensions = new List<string>();
				extensions.AddRange(EnvironmentHelper.HeaderFileExtensions);
				extensions.AddRange(EnvironmentHelper.CompileFileExtensions);
				extensions.AddRange(EnvironmentHelper.CSharpFileExtensions);
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
			}

			File.WriteAllText(hashesFilePath, hashesData.Content);

			return result;
		}

		private static void AddAllInclusionsFromDependencies(CPPProject.Profile Profile, SourceBuilder Builder)
		{
			if (Builder == null)
				return;

			if (Builder.SelectedRule.LibraryUseType != BuildRules.LibraryUseTypes.UseOnly)
			{
				Profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(Builder.SelectedRule.TargetName, (Builder.SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.DynamicLibrary ? BuildSystemHelper.APIPreprocessorTypes.Import : BuildSystemHelper.APIPreprocessorTypes.Empty)));
				Profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(Builder.SelectedRule.TargetName, BuildSystemHelper.ExternPreprocessorTypes.Empty));

				string[] libFiles = FileSystemUtilites.GetAllFiles(Builder.BinariesPath, "*" + EnvironmentHelper.StaticLibraryExtentions);

				if (libFiles != null)
					foreach (string libFile in libFiles)
						Profile.AddIncludeLibraries(libFile);
			}


			if (Builder.SelectedRule.DependencyModulesName == null)
				return;

			foreach (string dep in Builder.SelectedRule.DependencyModulesName)
			{
				SourceBuilder builder = BuildSystem.GetSourceBuilder(dep);

				AddAllInclusionsFromDependencies(Profile, builder);
			}
		}

		private static void AddDependency(CPPProject.Profile Profile, string Dependency)
		{
			SourceBuilder builder = BuildSystem.GetSourceBuilder(Dependency);

			if (builder == null)
				return;

			Profile.AddIncludeDirectories(FileSystemUtilites.GetParentDirectory(builder.sourcePathRoot));
			Profile.AddIncludeDirectories(FileSystemUtilites.PathSeperatorCorrection(builder.sourcePathRoot));

			if (builder.SelectedRule.GenerateReflection)
			{
				Profile.AddIncludeDirectories(builder.generatedFilesPath);

				AddDependency(Profile, BuildSystemHelper.ReflectionModuleName);
			}

			if (builder.SelectedRule.IncludesPath != null)
				foreach (string includePath in builder.SelectedRule.IncludesPath)
					Profile.AddIncludeDirectories(FileSystemUtilites.PathSeperatorCorrection(builder.sourcePathRoot + includePath));

			if (builder.SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.UseOnly)
			{
				string[] temp = builder.SelectedRule.LibrariesPath;

				if (temp != null)
					foreach (string file in temp)
						Profile.AddIncludeLibraries(builder.sourcePathRoot + FileSystemUtilites.PathSeperatorCorrection(file));
			}
			//else
			//{
			//	profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(builder.SelectedRule.TargetName, BuildSystemHelper.APIPreprocessorValues.Import));

			//	string[] libFiles = FileSystemUtilites.GetAllFiles(builder.BinariesPath, "*" + EnvironmentHelper.StaticLibraryExtentions);

			//	if (libFiles != null)
			//		foreach (string libFile in libFiles)
			//			profile.AddIncludeLibraries(libFile);
			//}

			AddAllInclusionsFromDependencies(Profile, builder);
		}

		private static void CopyAllFilesToFinalPath(string SourcePath, string Extension)
		{
			FileSystemUtilites.CopyAllFiles(SourcePath, EnvironmentHelper.FinalOutputDirectory, true, "*" + Extension);
		}

		private static int GetHash(string Value)
		{
			return Value.GetHashCode();
		}

		private static string GetExtension(SourceBuilder Builder)
		{
			switch (Builder.SelectedRule.LibraryUseType)
			{
				case BuildRules.LibraryUseTypes.Executable:
					return EnvironmentHelper.ExecutableExtentions;

				case BuildRules.LibraryUseTypes.DynamicLibrary:
					return EnvironmentHelper.DynamicLibraryExtentions;

				case BuildRules.LibraryUseTypes.StaticLibrary:
					return EnvironmentHelper.StaticLibraryExtentions;
			}

			return "";
		}
	}
}
