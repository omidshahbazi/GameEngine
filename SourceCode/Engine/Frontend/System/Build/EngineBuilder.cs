// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using GameFramework.ASCIISerializer;
using GameFramework.Common.Utilities;
using System;
using System.Collections.Generic;
using System.IO;

namespace Engine.Frontend.System.Build
{
	delegate void NewWrapperFileEventHandler(string FilePath);

	class EngineBuilder : BuilderBase
	{
		private const string HashesFileName = "Hash.cache";

		private string sourcePathRoot = "";
		protected static CommandLineProcess reflectionGeneratorProcess = null;
		protected static CommandLineProcess wrapperGeneratorProcess = null;

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

		public event NewWrapperFileEventHandler OnNewWrapperFile;

		public EngineBuilder(BuildRules Rules, string SourcePathRoot) :
			base(Rules.ModuleName)
		{
			BuildRule = Rules;

			foreach (BuildRules.RuleBase rule in BuildRule.Rules)
			{
				if (((int)rule.Configuration & (int)BuildSystem.BuildConfiguration) != (int)BuildSystem.BuildConfiguration)
					continue;

				if (((int)rule.Platform & (int)BuildSystem.PlatformArchitecture) != (int)BuildSystem.PlatformArchitecture)
					continue;

				SelectedRule = rule;
				break;
			}

			sourcePathRoot = SourcePathRoot;
		}

		public override bool Build(bool ForceToRebuild)
		{
			if (State == States.Built || State == States.AlreadyUpdated)
				return true;
			else if (State == States.Failed)
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
							ConsoleHelper.WriteError("Couldn't find file [" + srcFilePath + "]");
							State = States.Failed;
							break;
						}

						string dstFilePath = EnvironmentHelper.FinalOutputDirectory + Path.GetFileName(file);

						File.Copy(srcFilePath, dstFilePath, true);
					}
				}

				State = States.Built;
			}
			else if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.ProjectFile)
				BuildProjectFile();
			else
				GenerateAndBuildProjectFile(ForceToRebuild);

			return (State == States.Built || State == States.AlreadyUpdated);
		}

		private void GenerateAndBuildProjectFile(bool ForceToRebuild)
		{
			LogCurrentInfo();

			if (!ForceToRebuild && !MustCompile())
			{
				State = States.AlreadyUpdated;
				return;
			}

			CPPProject cppProj = new CPPProject();
			CPPProject.Profile profile = (CPPProject.Profile)cppProj.CreateProfile();

			profile.AssemblyName = SelectedRule.TargetName;
			profile.OutputType = BuildSystemHelper.LibraryUseTypesToOutputType(SelectedRule.LibraryUseType);
			profile.OutputPath = BinariesPath;
			profile.Optimization = CPPProject.Profile.Optimizations.Disabled;
			profile.MinimalRebuild = false;
			profile.LanguageStandard = CPPProject.Profile.LanguageStandards.CPP14;
			profile.PlatformArchitecture = BuildSystem.PlatformArchitecture;
			profile.BuildConfiguration = BuildSystem.BuildConfiguration;

			if (profile.BuildConfiguration == ProjectBase.ProfileBase.BuildConfigurations.Debug)
			{
				profile.GenerateDebugInformation = true;

				//if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
				//	profile.RuntimeLibrary = CPPProject.Profile.RuntimeLibraries.MultiThreadedDebug;
				//else
				profile.RuntimeLibrary = CPPProject.Profile.RuntimeLibraries.MultiThreadedDebugDLL;
			}
			else if (profile.BuildConfiguration == ProjectBase.ProfileBase.BuildConfigurations.Release)
			{
				profile.GenerateDebugInformation = false;

				//if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
				//	profile.RuntimeLibrary = CPPProject.Profile.RuntimeLibraries.MultiThreaded;
				//else
				profile.RuntimeLibrary = CPPProject.Profile.RuntimeLibraries.MultiThreadedDLL;
			}

			profile.AddIncludeDirectories(EnvironmentHelper.ProcessDirectory);
			profile.AddIncludeDirectories(sourcePathRoot);
			profile.AddIncludeDirectories(GeneratedFilesPath);
			if (SelectedRule.DependencyModulesName != null)
			{
				foreach (string dep in SelectedRule.DependencyModulesName)
					AddDependency(profile, dep);
			}

			if (SelectedRule.GenerateReflection)
				if (!AddDependency(profile, BuildSystemHelper.ReflectionModuleName))
					return;

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetModuleNamePreprocessor(BuildRule.ModuleName));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(BuildRule.ModuleName, BuildSystemHelper.APIPreprocessorTypes.Export));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(BuildRule.ModuleName, BuildSystemHelper.ExternPreprocessorTypes.Fill));
			if (SelectedRule.PreprocessorDefinitions != null)
				foreach (string def in SelectedRule.PreprocessorDefinitions)
					profile.AddPreprocessorDefinition(def);

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetConfigurationModePreprocessor(BuildSystem.BuildConfiguration));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformPreprocessor(EnvironmentHelper.Platform));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformTypesPreprocessor(BuildSystem.PlatformArchitecture));

			if (SelectedRule.DependencyStaticLibraries != null)
				foreach (string lib in SelectedRule.DependencyStaticLibraries)
					profile.AddIncludeLibraries(lib);

			string[] files = FileSystemUtilites.GetAllFiles(sourcePathRoot, EnvironmentHelper.HeaderFileExtensions);
			foreach (string file in files)
			{
				cppProj.AddIncludeFile(file);

				string outputBasePath = GeneratedFilesPath + Path.GetFileNameWithoutExtension(file);

				if (SelectedRule.GenerateReflection)
				{
					string outputBaseFileName = outputBasePath + ".Reflection";
					if (ParseForReflection(file, outputBaseFileName))
					{
						cppProj.AddIncludeFile(outputBaseFileName + EnvironmentHelper.HeaderFileExtension);
						cppProj.AddCompileFile(outputBaseFileName + EnvironmentHelper.CompileFileExtension);
					}
				}

				if (SelectedRule.GenerateWrapper)
				{
					string outputBaseFileName = outputBasePath + ".Wrapper";
					if (ParseForWrapper(file, outputBaseFileName))
					{
						cppProj.AddIncludeFile(outputBaseFileName + EnvironmentHelper.HeaderFileExtension);
						cppProj.AddCompileFile(outputBaseFileName + EnvironmentHelper.CompileFileExtension);

						if (OnNewWrapperFile != null)
							OnNewWrapperFile(outputBaseFileName + EnvironmentHelper.CSharpFileExtension);
					}
				}
			}

			if (SelectedRule.IncludeModulesName != null)
			{
				foreach (string moduleName in SelectedRule.IncludeModulesName)
				{
					EngineBuilder builder = BuildSystem.GetEngineBuilder(moduleName);

					if (builder == null)
						continue;

					profile.AddIncludeDirectories(builder.sourcePathRoot);

					if (BuildRule.ModuleName != moduleName)
					{
						profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(builder.BuildRule.ModuleName, BuildSystemHelper.APIPreprocessorTypes.Empty));
						profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(builder.BuildRule.ModuleName, BuildSystemHelper.ExternPreprocessorTypes.Empty));
					}
				}
			}

			if (SelectedRule.AdditionalIncludeDirectory != null)
				foreach (string dir in SelectedRule.AdditionalIncludeDirectory)
				{
					string dirInfo = FileSystemUtilites.PathSeperatorCorrection(dir);

					if (dirInfo.StartsWith("" + EnvironmentHelper.PathSeparator))
						dirInfo = dirInfo.Substring(1);

					profile.AddIncludeDirectories(sourcePathRoot + dirInfo);
				}

			files = FileSystemUtilites.GetAllFiles(sourcePathRoot, EnvironmentHelper.CompileFileExtensions);
			foreach (string file in files)
				cppProj.AddCompileFile(file);

			if (SelectedRule.AdditionalCompileFile != null)
				foreach (string file in SelectedRule.AdditionalCompileFile)
					cppProj.AddCompileFile(EnvironmentHelper.ProcessDirectory + file);

			profile.IntermediatePath = IntermediateModulePath;

			BuildProjectFile(profile);
		}

		private void BuildProjectFile()
		{
			LogCurrentInfo();

			if (Compile(GeneratedFilesPath))
			{
				if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
					CopyAllFilesToFinalPath(BinariesPath, EnvironmentHelper.ExecutableExtentions);
				else if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.DynamicLibrary)
					CopyAllFilesToFinalPath(BinariesPath, EnvironmentHelper.DynamicLibraryExtentions);

				State = States.Built;

				return;
			}

			ConsoleHelper.WriteError("Building " + BuildRule.ModuleName + " failed");

			State = States.Failed;
		}

		private void BuildProjectFile(ProjectBase.ProfileBase ProjectProfile)
		{
			if (Compile(ProjectProfile))
			{
				if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.Executable)
					CopyAllFilesToFinalPath(BinariesPath, EnvironmentHelper.ExecutableExtentions);
				else if (SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.DynamicLibrary)
					CopyAllFilesToFinalPath(BinariesPath, EnvironmentHelper.DynamicLibraryExtentions);

				State = States.Built;

				return;
			}

			ConsoleHelper.WriteError("Building " + BuildRule.ModuleName + " failed");

			State = States.Failed;
		}

		private bool ParseForReflection(string FilePath, string OutputBaseFileName)
		{
			if (reflectionGeneratorProcess == null)
			{
				if (!File.Exists(EnvironmentHelper.ReflectionToolPath))
					return false;

				reflectionGeneratorProcess = new CommandLineProcess();
				reflectionGeneratorProcess.FilePath = EnvironmentHelper.ReflectionToolPath;
			}

			reflectionGeneratorProcess.Start("\"" + FilePath + "\" \"" + OutputBaseFileName + "\"");

			string error = "";

			while (!reflectionGeneratorProcess.Output.EndOfStream)
				error += reflectionGeneratorProcess.Output.ReadLine() + Environment.NewLine;

			if (reflectionGeneratorProcess.ExitCode == 0)
				return true;
			else if (reflectionGeneratorProcess.ExitCode == 1)
				return false;

			throw new ApplicationException(error);
		}

		private bool ParseForWrapper(string FilePath, string OutputBaseFileName)
		{
			if (wrapperGeneratorProcess == null)
			{
				if (!File.Exists(EnvironmentHelper.WrapperToolPath))
					return false;

				wrapperGeneratorProcess = new CommandLineProcess();
				wrapperGeneratorProcess.FilePath = EnvironmentHelper.WrapperToolPath;
			}

			wrapperGeneratorProcess.Start(SelectedRule.TargetName + EnvironmentHelper.DynamicLibraryExtentions + " " + BuildSystemHelper.GetAPIPreprocessorName(BuildRule.ModuleName) + " \"" + FilePath + "\" \"" + OutputBaseFileName + "\"");

			while (!wrapperGeneratorProcess.Output.EndOfStream)
				wrapperGeneratorProcess.Output.ReadLine();

			return (wrapperGeneratorProcess.ExitCode == 0);
		}

		private bool MustCompile()
		{
			if (!File.Exists(EnvironmentHelper.FinalOutputDirectory + SelectedRule.TargetName + GetExtension(this)))
				return true;

			string hashesFilePath = IntermediateModulePath + HashesFileName;

			ISerializeObject hashesData = null;

			if (File.Exists(hashesFilePath))
				hashesData = Creator.Create<ISerializeObject>(File.ReadAllText(hashesFilePath));

			if (hashesData == null)
				hashesData = Creator.Create<ISerializeObject>();

			bool result = false;


			string configurationTypeName = typeof(ProjectBase.ProfileBase.BuildConfigurations).Name;
			if (!hashesData.Contains(configurationTypeName) || hashesData.Get<int>(configurationTypeName) != (int)BuildSystem.BuildConfiguration)
				result = true;
			hashesData.Set(configurationTypeName, (int)BuildSystem.BuildConfiguration);

			string platformTypeName = typeof(ProjectBase.ProfileBase.PlatformArchitectures).Name;
			if (!hashesData.Contains(platformTypeName) || hashesData.Get<int>(platformTypeName) != (int)BuildSystem.PlatformArchitecture)
				result = true;
			hashesData.Set(platformTypeName, (int)BuildSystem.PlatformArchitecture);

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

		private bool AddDependency(CPPProject.Profile Profile, string Dependency)
		{
			if (BuildRule.ModuleName == Dependency)
			{
				ConsoleHelper.WriteError("Module [" + BuildRule.ModuleName + "] set as its dependency");
				
				return false;
			}

			EngineBuilder builder = BuildSystem.GetEngineBuilder(Dependency);

			if (builder == null)
			{
				ConsoleHelper.WriteError("Dependency [" + Dependency + "] not found");

				return false;
			}

			Profile.AddIncludeDirectories(FileSystemUtilites.GetParentDirectory(builder.sourcePathRoot));

			//TODO: maybe we can remove this now, check it out
			Profile.AddIncludeDirectories(FileSystemUtilites.PathSeperatorCorrection(builder.sourcePathRoot));

			if (builder.SelectedRule.GenerateReflection)
			{
				Profile.AddIncludeDirectories(builder.GeneratedFilesPath);

				if (!AddDependency(Profile, BuildSystemHelper.ReflectionModuleName))
					return false;
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

			AddAllInclusionsFromDependencies(Profile, builder);

			return true;
		}

		private static void AddAllInclusionsFromDependencies(CPPProject.Profile Profile, EngineBuilder Builder)
		{
			if (Builder == null)
				return;

			if (Builder.SelectedRule.LibraryUseType != BuildRules.LibraryUseTypes.UseOnly)
			{
				Profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(Builder.BuildRule.ModuleName, (Builder.SelectedRule.LibraryUseType == BuildRules.LibraryUseTypes.DynamicLibrary ? BuildSystemHelper.APIPreprocessorTypes.Import : BuildSystemHelper.APIPreprocessorTypes.Empty)));
				Profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(Builder.BuildRule.ModuleName, BuildSystemHelper.ExternPreprocessorTypes.Empty));

				string[] libFiles = FileSystemUtilites.GetAllFiles(Builder.BinariesPath, "*" + EnvironmentHelper.StaticLibraryExtentions);

				if (libFiles != null)
					foreach (string libFile in libFiles)
						Profile.AddIncludeLibraries(libFile);
			}


			if (Builder.SelectedRule.DependencyModulesName == null)
				return;

			foreach (string dep in Builder.SelectedRule.DependencyModulesName)
			{
				EngineBuilder builder = BuildSystem.GetEngineBuilder(dep);

				AddAllInclusionsFromDependencies(Profile, builder);
			}
		}
	}
}
