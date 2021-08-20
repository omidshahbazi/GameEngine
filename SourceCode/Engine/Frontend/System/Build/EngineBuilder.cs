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

		public ModuleRules BuildRule
		{
			get;
			private set;
		}

		public ModuleRules.BuildRulesBase SelectedRule
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

				if (SelectedRule.LibraryUseType == ModuleRules.LibraryUseTypes.StaticLibrary)
					path += IntermediateBinariesPath;
				else
					path += EnvironmentHelper.FinalOutputDirectory;

				return path + SelectedRule.TargetName;
			}
		}

		public EngineBuilder(ModuleRules Rules, string SourcePathRoot) :
			base(Rules.Name)
		{
			BuildRule = Rules;

			foreach (ModuleRules.BuildRulesBase rule in BuildRule.BuildRules)
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

			if (SelectedRule.LibraryUseType == ModuleRules.LibraryUseTypes.UseOnly)
			{
				if (SelectedRule.BinariesPath != null)
				{
					foreach (string file in SelectedRule.BinariesPath)
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
			else if (SelectedRule.LibraryUseType == ModuleRules.LibraryUseTypes.ProjectFile)
				BuildProjectFile();
			else
				GenerateAndBuildProjectFile(ForceToRebuild);

			if (SelectedRule.GenerateRenderDocSettings)
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

			profile.AssemblyName = SelectedRule.TargetName;
			profile.OutputType = BuildSystemHelper.LibraryUseTypesToOutputType(SelectedRule.LibraryUseType);
			profile.OutputPath = IntermediateBinariesPath;
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

			profile.AddIncludeDirectories(FileSystemUtilites.GetParentDirectory(sourcePathRoot));

			if (SelectedRule.GenerateReflection)
				profile.AddIncludeDirectories(GeneratedFilesPath);

			if (SelectedRule.PrivateDependencyModulesName != null)
				foreach (string dep in SelectedRule.PrivateDependencyModulesName)
					AddDependency(profile, dep);

			if (SelectedRule.PublicDependencyModulesName != null)
				foreach (string dep in SelectedRule.PublicDependencyModulesName)
					AddDependency(profile, dep);

			if (SelectedRule.GenerateReflection)
				if (!AddDependency(profile, BuildSystemHelper.ReflectionModuleName))
					return;

			profile.AddPreprocessorDefinition(BuildSystemHelper.GetModuleNamePreprocessor(BuildRule.Name));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(BuildRule.Name, BuildSystemHelper.APIPreprocessorTypes.Export));
			profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(BuildRule.Name, BuildSystemHelper.ExternPreprocessorTypes.Fill));
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

					if (BuildRule.Name != moduleName)
					{
						profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(builder.BuildRule.Name, BuildSystemHelper.APIPreprocessorTypes.Empty));
						profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(builder.BuildRule.Name, BuildSystemHelper.ExternPreprocessorTypes.Empty));
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
				if (SelectedRule.LibraryUseType == ModuleRules.LibraryUseTypes.Executable)
					CopyAllFilesToFinalPath(IntermediateBinariesPath, EnvironmentHelper.ExecutableExtentions);
				else if (SelectedRule.LibraryUseType == ModuleRules.LibraryUseTypes.DynamicLibrary)
					CopyAllFilesToFinalPath(IntermediateBinariesPath, EnvironmentHelper.DynamicLibraryExtentions);

				State = States.Built;

				return;
			}

			ConsoleHelper.WriteError("Building " + BuildRule.Name + " failed");

			State = States.Failed;
		}

		private void BuildProjectFile(ProjectBase.ProfileBase ProjectProfile)
		{
			if (Compile(ProjectProfile))
			{
				if (SelectedRule.LibraryUseType == ModuleRules.LibraryUseTypes.Executable)
					CopyAllFilesToFinalPath(IntermediateBinariesPath, EnvironmentHelper.ExecutableExtentions);
				else if (SelectedRule.LibraryUseType == ModuleRules.LibraryUseTypes.DynamicLibrary)
					CopyAllFilesToFinalPath(IntermediateBinariesPath, EnvironmentHelper.DynamicLibraryExtentions);

				State = States.Built;

				return;
			}

			ConsoleHelper.WriteError("Building " + BuildRule.Name + " failed");

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

			wrapperGeneratorProcess.Start(SelectedRule.TargetName + EnvironmentHelper.DynamicLibraryExtentions + " " + BuildSystemHelper.GetAPIPreprocessorName(BuildRule.Name) + " \"" + FilePath + "\" \"" + OutputBaseFileName + "\"");

			while (!wrapperGeneratorProcess.Output.EndOfStream)
				wrapperGeneratorProcess.Output.ReadLine();

			return (wrapperGeneratorProcess.ExitCode == 0);
		}

		private bool GenerateRenderDocSettings()
		{
			if (SelectedRule.LibraryUseType != ModuleRules.LibraryUseTypes.Executable)
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
			if (BuildRule.Name == Dependency)
			{
				ConsoleHelper.WriteError("Module [" + BuildRule.Name + "] set as its dependency");

				return false;
			}

			EngineBuilder builder = BuildSystem.GetEngineBuilder(Dependency);

			if (builder == null)
			{
				ConsoleHelper.WriteError("Dependency [" + Dependency + "] not found");

				return false;
			}

			if (builder.SelectedRule.IncludesPath != null)
				foreach (string includePath in builder.SelectedRule.IncludesPath)
					Profile.AddIncludeDirectories(FileSystemUtilites.PathSeperatorCorrection(builder.sourcePathRoot + includePath));

			if (builder.SelectedRule.LibraryUseType == ModuleRules.LibraryUseTypes.UseOnly)
			{
				string[] temp = builder.SelectedRule.LibrariesPath;

				if (temp != null)
					foreach (string file in temp)
						Profile.AddIncludeLibraries(builder.sourcePathRoot + FileSystemUtilites.PathSeperatorCorrection(file));
			}

			AddAllInclusionsFromDependencies(Profile, builder);

			return true;
		}

		private void AddAllInclusionsFromDependencies(CPPProject.Profile Profile, EngineBuilder Builder)
		{
			if (Builder == null)
				return;

			Profile.AddIncludeDirectories(FileSystemUtilites.GetParentDirectory(Builder.sourcePathRoot));

			if (Builder.SelectedRule.GenerateReflection)
			{
				Profile.AddIncludeDirectories(Builder.GeneratedFilesPath);

				if (!AddDependency(Profile, BuildSystemHelper.ReflectionModuleName))
					return;
			}


			if (Builder.SelectedRule.LibraryUseType != ModuleRules.LibraryUseTypes.UseOnly)
			{
				Profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(Builder.BuildRule.Name, (Builder.SelectedRule.LibraryUseType == ModuleRules.LibraryUseTypes.DynamicLibrary ? BuildSystemHelper.APIPreprocessorTypes.Import : BuildSystemHelper.APIPreprocessorTypes.Empty)));
				Profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(Builder.BuildRule.Name, BuildSystemHelper.ExternPreprocessorTypes.Empty));

				string[] libFiles = FileSystemUtilites.GetAllFiles(Builder.IntermediateBinariesPath, "*" + EnvironmentHelper.StaticLibraryExtentions);

				if (libFiles != null)
					foreach (string libFile in libFiles)
						Profile.AddIncludeLibraries(libFile);
			}

			if (Builder.SelectedRule.PublicDependencyModulesName != null)
				foreach (string dep in Builder.SelectedRule.PublicDependencyModulesName)
				{
					EngineBuilder builder = BuildSystem.GetEngineBuilder(dep);

					AddAllInclusionsFromDependencies(Profile, builder);
				}
		}
	}
}
