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
	class EngineBuilder : BuilderBase
	{
		private const string HashesFileName = "Hash.cache";

		private string sourcePathRoot = "";
		protected static ReflectionBuildProcess reflectionBuildProcess = null;

		public ModuleRules Module
		{
			get;
			private set;
		}

		public ModuleRules.BuildRulesBase BuildRules
		{
			get;
			private set;
		}

		protected string OutputTargetName
		{
			get
			{
				string path = "";

				if (BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.StaticLibrary)
					path += IntermediateOutputPaths;
				else
					path += EnvironmentHelper.OutputDirectory;

				return path + BuildRules.TargetName;
			}
		}

		protected override string ModuleName
		{
			get { return Module.Name; }
		}

		public EngineBuilder(ModuleRules Module)
		{
			this.Module = Module;

			foreach (ModuleRules.BuildRulesBase build in this.Module.BuildRules)
			{
				if (((int)build.Configuration & (int)BuildSystemHelper.BuildConfiguration) != (int)BuildSystemHelper.BuildConfiguration)
					continue;

				if (((int)build.Platform & (int)BuildSystemHelper.PlatformArchitecture) != (int)BuildSystemHelper.PlatformArchitecture)
					continue;

				BuildRules = build;

				break;
			}

			sourcePathRoot = Module.GetSourceRootDirectory();
		}

		public override void Build(bool ForceToRebuild)
		{
			if (State != States.NotBuilt)
				return;

			if (BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.UseOnly)
			{
				if (BuildRules.BinaryPaths != null)
				{
					foreach (string file in BuildRules.BinaryPaths)
					{
						string srcFilePath = sourcePathRoot + FileSystemUtilites.PathSeperatorCorrection(file);

						if (!File.Exists(srcFilePath))
						{
							ConsoleHelper.WriteError($"Couldn't find file {srcFilePath}");
							State = States.Failed;
							break;
						}

						string dstFilePath = EnvironmentHelper.OutputDirectory + Path.GetFileName(file);

						File.Copy(srcFilePath, dstFilePath, true);
					}
				}

				State = States.Built;
			}
			else if (BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.ProjectFile)
				BuildProjectFile();
			else
				GenerateAndBuildProjectFile(ForceToRebuild);

			if (BuildRules.GenerateRenderDocSettings)
				GenerateRenderDocSettings();
		}

		protected override void CreateDirectories()
		{
			base.CreateDirectories();

			if (BuildRules.LibraryUseType != ModuleRules.LibraryUseTypes.UseOnly)
			{
				if (!Directory.Exists(GeneratedFilesPath))
					Directory.CreateDirectory(GeneratedFilesPath);
			}
		}

		private void GenerateAndBuildProjectFile(bool ForceToRebuild)
		{
			LogCurrentInfo();

			if (!ForceToRebuild && !MustCompile())
			{
				State = States.AlreadyUpdated;
				return;
			}

			CreateDirectories();

			CPPProject cppProj = new CPPProject();
			CPPProject.Profile profile = (CPPProject.Profile)cppProj.CreateProfile();

			profile.AssemblyName = BuildRules.TargetName;
			profile.OutputType = BuildSystemHelper.GetLibraryUseTypesToOutputType(BuildRules.LibraryUseType);
			profile.OutputPath = IntermediateOutputPaths;
			profile.Optimization = CPPProject.Profile.Optimizations.Disabled;
			profile.MinimalRebuild = false;
			profile.LanguageStandard = CPPProject.Profile.LanguageStandards.CPPLatest;
			profile.PlatformArchitecture = BuildSystemHelper.PlatformArchitecture;
			profile.BuildConfiguration = BuildSystemHelper.BuildConfiguration;

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

			profile.AddIncludeDirectory(FileSystemUtilites.GetParentDirectory(sourcePathRoot));

			if (BuildRules.GenerateReflection)
				profile.AddIncludeDirectory(GeneratedFilesPath);

			if (BuildRules.PrivateDependencyModuleNames != null)
				foreach (string dep in BuildRules.PrivateDependencyModuleNames)
					AddDependency(profile, dep);

			if (BuildRules.PublicDependencyModuleNames != null)
				foreach (string dep in BuildRules.PublicDependencyModuleNames)
					AddDependency(profile, dep);

			if (BuildRules.GenerateReflection)
				AddDependency(profile, EnvironmentHelper.ReflectionModuleName);

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetExportAPIPreprocessorRaw());
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetModuleNamePreprocessor(Module.Name));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(Module.Name, BuildSystemHelper.APIPreprocessorTypes.Export));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(Module.Name, BuildSystemHelper.ExternPreprocessorTypes.Fill));
			if (BuildRules.PreprocessorDefinitions != null)
				foreach (string def in BuildRules.PreprocessorDefinitions)
					profile.AddPreprocessorDefinition(def);

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetConfigurationModePreprocessor(BuildSystemHelper.BuildConfiguration));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformPreprocessor(EnvironmentHelper.Platform));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformTypesPreprocessor(BuildSystemHelper.PlatformArchitecture));

			if (BuildRules.DependencyStaticLibraries != null)
				foreach (string lib in BuildRules.DependencyStaticLibraries)
					profile.AddIncludeLibrary(lib);

			string[] files = FileSystemUtilites.GetAllFiles(sourcePathRoot, EnvironmentHelper.HeaderFileExtensions);
			foreach (string file in files)
			{
				cppProj.AddIncludeFile(file);

				string outputBasePath = GeneratedFilesPath + Path.GetFileNameWithoutExtension(file);

				if (BuildRules.GenerateReflection)
				{
					string outputBaseFileName = outputBasePath + ".Reflection";
					if (ParseForReflection(file, outputBaseFileName))
					{
						cppProj.AddIncludeFile(outputBaseFileName + EnvironmentHelper.HeaderFileExtension);
						cppProj.AddCompileFile(outputBaseFileName + EnvironmentHelper.CompileFileExtension);
					}
				}
			}

			files = FileSystemUtilites.GetAllFiles(sourcePathRoot, EnvironmentHelper.CompileFileExtensions);
			foreach (string file in files)
				cppProj.AddCompileFile(file);

			profile.IntermediatePath = IntermediateModulePath;

			BuildProjectFile(profile);
		}

		private void BuildProjectFile()
		{
			LogCurrentInfo();

			try
			{
				Compile(GeneratedFilesPath);

				if (BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.Executable)
					CopyAllFilesToFinalPath(IntermediateOutputPaths, EnvironmentHelper.ExecutableExtentions);
				else if (BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.DynamicLibrary)
					CopyAllFilesToFinalPath(IntermediateOutputPaths, EnvironmentHelper.DynamicLibraryExtentions);

				State = States.Built;
			}
			catch
			{
				ConsoleHelper.WriteError($"Building {Module.Name} has failed");

				State = States.Failed;
			}
		}

		private void BuildProjectFile(ProjectBase.ProfileBase ProjectProfile)
		{
			try
			{
				Compile(ProjectProfile);

				if (BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.Executable)
					CopyAllFilesToFinalPath(IntermediateOutputPaths, EnvironmentHelper.ExecutableExtentions);
				else if (BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.DynamicLibrary)
					CopyAllFilesToFinalPath(IntermediateOutputPaths, EnvironmentHelper.DynamicLibraryExtentions);

				State = States.Built;
			}
			catch
			{
				ConsoleHelper.WriteError($"Building {Module.Name} has failed");

				State = States.Failed;
			}
		}

		private bool ParseForReflection(string FilePath, string OutputBaseFileName)
		{
			if (reflectionBuildProcess == null)
				reflectionBuildProcess = new ReflectionBuildProcess();

			return reflectionBuildProcess.Build(FilePath, OutputBaseFileName);
		}

		private void GenerateRenderDocSettings()
		{
			if (BuildRules.LibraryUseType != ModuleRules.LibraryUseTypes.Executable)
				throw new FrontendException($"Cannot generate RenderDoc settings for {Module.Name} as it is a non-executable module");

			ISerializeObject rootObj = Creator.Create<ISerializeObject>();
			{
				rootObj["rdocCaptureSettings"] = 1;

				ISerializeObject settingsObj = rootObj.AddObject("settings");
				{
					settingsObj["executable"] = OutputTargetName + EnvironmentHelper.ExecutableExtentions;
					settingsObj["workingDir"] = EnvironmentHelper.OutputDirectory;

					settingsObj["autoStart"] = false;
					settingsObj["commandLine"] = "";
					settingsObj["inject"] = false;
					settingsObj["numQueuedFrames"] = 0;
					settingsObj["queuedFrameCap"] = 0;

					ISerializeArray environmentArr = settingsObj.AddArray("environment");
					{
					}

					ISerializeObject optionsObj = settingsObj.AddObject("options");
					{
						optionsObj["allowFullscreen"] = true;
						optionsObj["allowVSync"] = true;
						optionsObj["apiValidation"] = true;
						optionsObj["captureAllCmdLists"] = true;
						optionsObj["captureCallstacks"] = true;
						optionsObj["captureCallstacksOnlyDraws"] = false;
						optionsObj["debugOutputMute"] = true;
						optionsObj["delayForDebugger"] = 0;
						optionsObj["hookIntoChildren"] = false;
						optionsObj["refAllResources"] = true;
						optionsObj["verifyBufferAccess"] = true;
					}
				}
			}

			File.WriteAllText(OutputTargetName + ".RenderDoc.cap", rootObj.Content);
		}

		private bool MustCompile()
		{
			if (!File.Exists(OutputTargetName + BuildSystemHelper.GetExtension(BuildRules)))
				return true;

			string hashesFilePath = IntermediateModulePath + HashesFileName;

			ISerializeObject hashesData = null;

			if (File.Exists(hashesFilePath))
				hashesData = Creator.Create<ISerializeObject>(File.ReadAllText(hashesFilePath));

			if (hashesData == null)
				hashesData = Creator.Create<ISerializeObject>();

			bool result = false;

			string configurationTypeName = typeof(ProjectBase.ProfileBase.BuildConfigurations).Name;
			if (!hashesData.Contains(configurationTypeName) || hashesData.Get<int>(configurationTypeName) != (int)BuildSystemHelper.BuildConfiguration)
				result = true;
			hashesData.Set(configurationTypeName, (int)BuildSystemHelper.BuildConfiguration);

			string platformTypeName = typeof(ProjectBase.ProfileBase.PlatformArchitectures).Name;
			if (!hashesData.Contains(platformTypeName) || hashesData.Get<int>(platformTypeName) != (int)BuildSystemHelper.PlatformArchitecture)
				result = true;
			hashesData.Set(platformTypeName, (int)BuildSystemHelper.PlatformArchitecture);

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

		private void AddDependency(CPPProject.Profile Profile, string Dependency)
		{
			if (Module.Name == Dependency)
				throw new FrontendException($"Module {Module.Name} adds itself as dependency");

			EngineBuilder builder = BuildSystem.Instance.GetEngineBuilder(Dependency);

			if (builder.BuildRules.IncludePaths != null)
				foreach (string includePath in builder.BuildRules.IncludePaths)
					Profile.AddIncludeDirectory(FileSystemUtilites.PathSeperatorCorrection(builder.sourcePathRoot + includePath));

			if (builder.BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.UseOnly)
			{
				if (builder.BuildRules.LibraryPaths != null)
					foreach (string file in builder.BuildRules.LibraryPaths)
						Profile.AddIncludeLibrary(builder.sourcePathRoot + FileSystemUtilites.PathSeperatorCorrection(file));
			}

			AddAllInclusionsFromDependencies(Profile, builder);
		}

		private void AddAllInclusionsFromDependencies(CPPProject.Profile Profile, EngineBuilder Builder)
		{
			if (Builder == null)
				return;

			Profile.AddIncludeDirectory(FileSystemUtilites.GetParentDirectory(Builder.sourcePathRoot));

			if (Builder.BuildRules.GenerateReflection)
			{
				Profile.AddIncludeDirectory(Builder.GeneratedFilesPath);

				AddDependency(Profile, EnvironmentHelper.ReflectionModuleName);
			}

			if (Builder.BuildRules.LibraryUseType != ModuleRules.LibraryUseTypes.UseOnly)
			{
				Profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(Builder.Module.Name, (Builder.BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.DynamicLibrary ? BuildSystemHelper.APIPreprocessorTypes.Import : BuildSystemHelper.APIPreprocessorTypes.Empty)));
				Profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(Builder.Module.Name, BuildSystemHelper.ExternPreprocessorTypes.Empty));

				string[] libFiles = FileSystemUtilites.GetAllFiles(Builder.IntermediateOutputPaths, "*" + EnvironmentHelper.StaticLibraryExtentions);

				if (libFiles != null)
					foreach (string libFile in libFiles)
						Profile.AddIncludeLibrary(libFile);
			}

			if (Builder.BuildRules.PublicDependencyModuleNames != null)
				foreach (string dep in Builder.BuildRules.PublicDependencyModuleNames)
					AddAllInclusionsFromDependencies(Profile, BuildSystem.Instance.GetEngineBuilder(dep));
		}
	}
}