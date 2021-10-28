// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using GameFramework.ASCIISerializer;
using GameFramework.Common.Utilities;
using System.Collections.Generic;
using System.IO;

namespace Engine.Frontend.System.Build
{
	class EngineBuilder : BuilderBase
	{
		private string sourcePathRoot = "";
		protected static ReflectionBuildProcess reflectionBuildProcess = null;

		public ModuleRules Module
		{
			get;
			private set;
		}

		protected string OutputTargetName
		{
			get
			{
				string path = "";

				if (Module.LibraryUseType == ModuleRules.LibraryUseTypes.StaticLibrary)
					path += IntermediateOutputPath;
				else
					path += EnvironmentHelper.OutputDirectory;

				return path + Module.TargetName;
			}
		}

		protected override string ModuleName
		{
			get { return Module.Name; }
		}

		public EngineBuilder(ModuleRules Module)
		{
			this.Module = Module;

			sourcePathRoot = Module.GetSourceRootDirectory();

			Initialize();
		}

		public override void Build(bool ForceToRebuild)
		{
			if (State != States.NotBuilt)
				return;

			if (Module.LibraryUseType == ModuleRules.LibraryUseTypes.UseOnly)
			{
				foreach (string file in Module.BinaryPaths)
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

				State = States.AlreadyUpdated;
			}
			else if (Module.LibraryUseType == ModuleRules.LibraryUseTypes.ProjectFile)
				BuildProjectFile();
			else
				GenerateAndBuildProjectFile(ForceToRebuild);

			if (Module.GenerateRenderDocSettings)
				GenerateRenderDocSettings();
		}

		protected override void CreateDirectories()
		{
			base.CreateDirectories();

			if (Module.LibraryUseType != ModuleRules.LibraryUseTypes.UseOnly)
			{
				if (!Directory.Exists(IntermediateTempPath))
					Directory.CreateDirectory(IntermediateTempPath);

				if (!Directory.Exists(IntermediateGeneratedDirectory))
					Directory.CreateDirectory(IntermediateGeneratedDirectory);
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

			profile.AssemblyName = Module.TargetName;
			profile.OutputType = BuildSystemHelper.GetLibraryUseTypesToOutputType(Module.LibraryUseType);
			profile.OutputPath = IntermediateOutputPath;
			profile.MinimalRebuild = false;
			profile.LanguageStandard = CPPProject.Profile.LanguageStandards.CPPLatest;
			profile.PlatformArchitecture = BuildSystemHelper.PlatformArchitecture;
			profile.BuildConfiguration = BuildSystemHelper.BuildConfiguration;
			profile.IntermediateDirectory = IntermediateTempPath;

			if (profile.BuildConfiguration == ProjectBase.ProfileBase.BuildConfigurations.Debug)
			{
				profile.Optimization = CPPProject.Profile.Optimizations.Disabled;
				profile.GenerateDebugInfo = true;
				profile.RuntimeLibrary = CPPProject.Profile.RuntimeLibraries.MultiThreadedDebugDLL;
			}
			else if (profile.BuildConfiguration == ProjectBase.ProfileBase.BuildConfigurations.Release)
			{
				profile.Optimization = CPPProject.Profile.Optimizations.MaxSpeed;
				profile.GenerateDebugInfo = false;
				profile.RuntimeLibrary = CPPProject.Profile.RuntimeLibraries.MultiThreadedDLL;
			}

			//Override the release mode, cause we have an issue is PlatformGL::CreateWGLARBContext with optimization
			profile.Optimization = CPPProject.Profile.Optimizations.Disabled;

			profile.AddIncludeDirectory(FileSystemUtilites.GetParentDirectory(sourcePathRoot));

			if (Module.GenerateReflection)
				profile.AddIncludeDirectory(IntermediateGeneratedDirectory);

			foreach (string dep in Module.PrivateDependencyModuleNames)
				AddDependency(profile, dep);

			foreach (string dep in Module.PublicDependencyModuleNames)
				AddDependency(profile, dep);

			if (Module.GenerateReflection)
				AddDependency(profile, EnvironmentHelper.ReflectionModuleName);

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetExportAPIPreprocessorRaw());
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetModuleNamePreprocessor(Module.Name));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(Module.Name, BuildSystemHelper.APIPreprocessorTypes.Export));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(Module.Name, BuildSystemHelper.ExternPreprocessorTypes.Fill));

			foreach (string def in Module.PreprocessorDefinitions)
				profile.AddPreprocessorDefinition(def);

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetConfigurationModePreprocessor(BuildSystemHelper.BuildConfiguration));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetOperatingSystemPreprocessor(EnvironmentHelper.OperatingSystem));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformArchitecturePreprocessor(BuildSystemHelper.PlatformArchitecture));

			foreach (string lib in Module.DependencyStaticLibraries)
				profile.AddIncludeLibrary(lib);

			string[] files = FileSystemUtilites.GetAllFiles(sourcePathRoot, EnvironmentHelper.HeaderFileExtensions);
			foreach (string file in files)
			{
				cppProj.AddIncludeFile(file);

				if (Module.GenerateReflection)
				{
					string outputBaseFileName = IntermediateGeneratedDirectory + Path.GetFileNameWithoutExtension(file) + ".Reflection";
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

			BuildProjectFile(profile);
		}

		private void BuildProjectFile()
		{
			LogCurrentInfo();

			try
			{
				Compile(IntermediateGeneratedDirectory);

				if (Module.LibraryUseType == ModuleRules.LibraryUseTypes.Executable)
					CopyAllFilesToFinalPath(IntermediateOutputPath, EnvironmentHelper.ExecutableExtentions);
				else if (Module.LibraryUseType == ModuleRules.LibraryUseTypes.DynamicLibrary)
					CopyAllFilesToFinalPath(IntermediateOutputPath, EnvironmentHelper.DynamicLibraryExtentions);

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

				if (Module.LibraryUseType == ModuleRules.LibraryUseTypes.Executable)
					CopyAllFilesToFinalPath(IntermediateOutputPath, EnvironmentHelper.ExecutableExtentions);
				else if (Module.LibraryUseType == ModuleRules.LibraryUseTypes.DynamicLibrary)
					CopyAllFilesToFinalPath(IntermediateOutputPath, EnvironmentHelper.DynamicLibraryExtentions);

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
			if (Module.LibraryUseType != ModuleRules.LibraryUseTypes.Executable)
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
			if (!File.Exists(OutputTargetName + BuildSystemHelper.GetExtension(Module)))
				return true;

			string hashesFilePath = IntermediateTempPath + $"Hashes.cache";

			ISerializeObject hashesData = null;

			if (File.Exists(hashesFilePath))
				hashesData = Creator.Create<ISerializeObject>(File.ReadAllText(hashesFilePath));

			if (hashesData == null)
				hashesData = Creator.Create<ISerializeObject>();

			bool result = false;

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

			File.WriteAllText(hashesFilePath, hashesData.Content);

			return result;
		}

		private void AddDependency(CPPProject.Profile Profile, string Dependency)
		{
			if (Module.Name == Dependency)
				throw new FrontendException($"Module {Module.Name} adds itself as dependency");

			EngineBuilder builder = BuildSystem.Instance.GetEngineBuilder(Dependency);

			foreach (string includePath in builder.Module.IncludePaths)
				Profile.AddIncludeDirectory(FileSystemUtilites.PathSeperatorCorrection(builder.sourcePathRoot + includePath));

			if (builder.Module.LibraryUseType == ModuleRules.LibraryUseTypes.UseOnly)
			{
				foreach (string file in builder.Module.LibraryPaths)
					Profile.AddIncludeLibrary(builder.sourcePathRoot + FileSystemUtilites.PathSeperatorCorrection(file));
			}

			AddAllInclusionsFromDependencies(Profile, builder);
		}

		private void AddAllInclusionsFromDependencies(CPPProject.Profile Profile, EngineBuilder Builder)
		{
			if (Builder == null)
				return;

			Profile.AddIncludeDirectory(FileSystemUtilites.GetParentDirectory(Builder.sourcePathRoot));

			if (Builder.Module.LibraryUseType != ModuleRules.LibraryUseTypes.UseOnly)
			{
				if (Builder.Module.GenerateReflection)
				{
					Profile.AddIncludeDirectory(Builder.IntermediateGeneratedDirectory);

					AddDependency(Profile, EnvironmentHelper.ReflectionModuleName);
				}

				BuildSystemHelper.APIPreprocessorTypes type = BuildSystemHelper.APIPreprocessorTypes.Empty;
				switch (Builder.Module.LibraryUseType)
				{
					case ModuleRules.LibraryUseTypes.DynamicLibrary:
						type = BuildSystemHelper.APIPreprocessorTypes.Import;
						break;
				}

				Profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(Builder.Module.Name, type));
				Profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(Builder.Module.Name, BuildSystemHelper.ExternPreprocessorTypes.Empty));

				string[] libFiles = FileSystemUtilites.GetAllFiles(Builder.IntermediateOutputPath, "*" + EnvironmentHelper.StaticLibraryExtentions);

				foreach (string libFile in libFiles)
					Profile.AddIncludeLibrary(libFile);
			}

			foreach (string preprocessor in Builder.Module.PreprocessorDefinitions)
				Profile.AddPreprocessorDefinition(preprocessor);

			foreach (string dep in Builder.Module.PublicDependencyModuleNames)
				AddAllInclusionsFromDependencies(Profile, BuildSystem.Instance.GetEngineBuilder(dep));
		}
	}
}
