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

		public ModuleRules ModuleRules
		{
			get;
			private set;
		}

		public ModuleRules.BuildRulesBase BuildRules
		{
			get;
			private set;
		}

		public event NewWrapperFileEventHandler OnNewWrapperFile;

		protected string FinalOutputTargetName
		{
			get
			{
				string path = "";

				if (BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.StaticLibrary)
					path += IntermediateBinaryPaths;
				else
					path += EnvironmentHelper.FinalOutputDirectory;

				return path + BuildRules.TargetName;
			}
		}

		protected override string ModuleName
		{
			get { return ModuleRules.Name; }
		}

		public EngineBuilder(ModuleRules Rules, string SourcePathRoot)
		{
			ModuleRules = Rules;

			foreach (ModuleRules.BuildRulesBase rule in ModuleRules.BuildRules)
			{
				if (((int)rule.Configuration & (int)BuildSystem.BuildConfiguration) != (int)BuildSystem.BuildConfiguration)
					continue;

				if (((int)rule.Platform & (int)BuildSystem.PlatformArchitecture) != (int)BuildSystem.PlatformArchitecture)
					continue;

				BuildRules = rule;
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

			if (BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.UseOnly)
			{
				if (BuildRules.BinaryPaths != null)
				{
					foreach (string file in BuildRules.BinaryPaths)
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
			else if (BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.ProjectFile)
				BuildProjectFile();
			else
				GenerateAndBuildProjectFile(ForceToRebuild);

			if (BuildRules.GenerateRenderDocSettings)
				GenerateRenderDocSettings();

			return (State == States.Built || State == States.AlreadyUpdated);
		}

		protected override void CreateDirectories()
		{
			base.CreateDirectories();

			if (!Directory.Exists(GeneratedFilesPath))
				Directory.CreateDirectory(GeneratedFilesPath);
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
			profile.OutputType = BuildSystemHelper.LibraryUseTypesToOutputType(BuildRules.LibraryUseType);
			profile.OutputPath = IntermediateBinaryPaths;
			profile.Optimization = CPPProject.Profile.Optimizations.Disabled;
			profile.MinimalRebuild = false;
			profile.LanguageStandard = CPPProject.Profile.LanguageStandards.CPPLatest;
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
				if (!AddDependency(profile, BuildSystemHelper.ReflectionModuleName))
					return;

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetModuleNamePreprocessor(ModuleRules.Name));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(ModuleRules.Name, BuildSystemHelper.APIPreprocessorTypes.Export));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(ModuleRules.Name, BuildSystemHelper.ExternPreprocessorTypes.Fill));
			if (BuildRules.PreprocessorDefinitions != null)
				foreach (string def in BuildRules.PreprocessorDefinitions)
					profile.AddPreprocessorDefinition(def);

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetConfigurationModePreprocessor(BuildSystem.BuildConfiguration));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformPreprocessor(EnvironmentHelper.Platform));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformTypesPreprocessor(BuildSystem.PlatformArchitecture));

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

			if (Compile(GeneratedFilesPath))
			{
				if (BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.Executable)
					CopyAllFilesToFinalPath(IntermediateBinaryPaths, EnvironmentHelper.ExecutableExtentions);
				else if (BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.DynamicLibrary)
					CopyAllFilesToFinalPath(IntermediateBinaryPaths, EnvironmentHelper.DynamicLibraryExtentions);

				State = States.Built;

				return;
			}

			ConsoleHelper.WriteError("Building " + ModuleRules.Name + " failed");

			State = States.Failed;
		}

		private void BuildProjectFile(ProjectBase.ProfileBase ProjectProfile)
		{
			if (Compile(ProjectProfile))
			{
				if (BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.Executable)
					CopyAllFilesToFinalPath(IntermediateBinaryPaths, EnvironmentHelper.ExecutableExtentions);
				else if (BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.DynamicLibrary)
					CopyAllFilesToFinalPath(IntermediateBinaryPaths, EnvironmentHelper.DynamicLibraryExtentions);

				State = States.Built;

				return;
			}

			ConsoleHelper.WriteError("Building " + ModuleRules.Name + " failed");

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

			throw new ApplicationException("Parsing [" + FilePath + "] for reflection has error\n" + error);
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

			wrapperGeneratorProcess.Start(BuildRules.TargetName + EnvironmentHelper.DynamicLibraryExtentions + " " + BuildSystemHelper.GetAPIPreprocessorName(ModuleRules.Name) + " \"" + FilePath + "\" \"" + OutputBaseFileName + "\"");

			while (!wrapperGeneratorProcess.Output.EndOfStream)
				wrapperGeneratorProcess.Output.ReadLine();

			return (wrapperGeneratorProcess.ExitCode == 0);
		}

		private bool GenerateRenderDocSettings()
		{
			if (BuildRules.LibraryUseType != ModuleRules.LibraryUseTypes.Executable)
				return false;

			ISerializeObject rootObj = Creator.Create<ISerializeObject>();
			{
				rootObj["rdocCaptureSettings"] = 1;

				ISerializeObject settingsObj = rootObj.AddObject("settings");
				{
					settingsObj["executable"] = FinalOutputTargetName + EnvironmentHelper.ExecutableExtentions;
					settingsObj["workingDir"] = EnvironmentHelper.FinalOutputDirectory;

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
			File.WriteAllText(FinalOutputTargetName + ".RenderDoc.cap", rootObj.Content);

			return true;
		}

		private bool MustCompile()
		{
			if (!File.Exists(FinalOutputTargetName + GetExtension(this)))
				return (FileSystemUtilites.GetAllFiles(sourcePathRoot, EnvironmentHelper.CompileFileExtensions).Length != 0);

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
			if (ModuleRules.Name == Dependency)
			{
				ConsoleHelper.WriteError("Module [" + ModuleRules.Name + "] set as its dependency");

				return false;
			}

			EngineBuilder builder = BuildSystem.GetEngineBuilder(Dependency);

			if (builder == null)
			{
				ConsoleHelper.WriteError("Dependency [" + Dependency + "] not found");

				return false;
			}

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

			return true;
		}

		private void AddAllInclusionsFromDependencies(CPPProject.Profile Profile, EngineBuilder Builder)
		{
			if (Builder == null)
				return;

			Profile.AddIncludeDirectory(FileSystemUtilites.GetParentDirectory(Builder.sourcePathRoot));

			if (Builder.BuildRules.GenerateReflection)
			{
				Profile.AddIncludeDirectory(Builder.GeneratedFilesPath);

				if (!AddDependency(Profile, BuildSystemHelper.ReflectionModuleName))
					return;
			}

			if (Builder.BuildRules.LibraryUseType != ModuleRules.LibraryUseTypes.UseOnly)
			{
				Profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(Builder.ModuleRules.Name, (Builder.BuildRules.LibraryUseType == ModuleRules.LibraryUseTypes.DynamicLibrary ? BuildSystemHelper.APIPreprocessorTypes.Import : BuildSystemHelper.APIPreprocessorTypes.Empty)));
				Profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(Builder.ModuleRules.Name, BuildSystemHelper.ExternPreprocessorTypes.Empty));

				string[] libFiles = FileSystemUtilites.GetAllFiles(Builder.IntermediateBinaryPaths, "*" + EnvironmentHelper.StaticLibraryExtentions);

				if (libFiles != null)
					foreach (string libFile in libFiles)
						Profile.AddIncludeLibrary(libFile);
			}

			if (Builder.BuildRules.PublicDependencyModuleNames != null)
				foreach (string dep in Builder.BuildRules.PublicDependencyModuleNames)
				{
					EngineBuilder builder = BuildSystem.GetEngineBuilder(dep);

					AddAllInclusionsFromDependencies(Profile, builder);
				}
		}
	}
}
