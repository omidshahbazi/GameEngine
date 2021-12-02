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
	class ModuleBuilder : BaseBuilder
	{
		private TargetBuilder targetBuilder = null;
		private string sourceCodeRootPath = "";

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

				if (Module.UseType == ModuleRules.UseTypes.StaticLibrary)
					path += IntermediateOutputPath;
				else
					path += FinalOutputDirectory;

				return path + Module.TargetName;
			}
		}

		public override string ModuleName
		{
			get { return Module.Name; }
		}

		public ModuleBuilder(TargetBuilder TargetBuilder, ModuleRules Module) :
			base(TargetBuilder.Configuration, TargetBuilder.Architecture)
		{
			targetBuilder = TargetBuilder;
			this.Module = Module;

			sourceCodeRootPath = Module.GetSourceRootDirectory();

			Initialize();
		}

		public override void Build(bool ForceToRebuild)
		{
			if (State != States.NotBuilt)
				return;

			StartLoging();

			if (!ForceToRebuild && !MustCompile())
			{
				State = States.AlreadyUpdated;
				return;
			}

			try
			{
				if (Module.UseType == ModuleRules.UseTypes.UseOnly)
				{
					foreach (string file in Module.BinaryPaths)
					{
						string srcFilePath = sourceCodeRootPath + FileSystemUtilites.PathSeperatorCorrection(file);

						if (!File.Exists(srcFilePath))
						{
							ConsoleHelper.WriteError($"Couldn't find file {srcFilePath}");
							State = States.Failed;
							break;
						}

						string dstFilePath = FinalOutputDirectory + Path.GetFileName(file);

						File.Copy(srcFilePath, dstFilePath, true);
					}

					State = States.AlreadyUpdated;
				}
				else
				{
					CPPProject cppProject = GenerateProject();

					Compile(cppProject.Profiles[0]);

					if (Module.UseType == ModuleRules.UseTypes.Executable)
						CopyAllFilesToFinalPath(IntermediateOutputPath, EnvironmentHelper.ExecutableExtentions);
					else if (Module.UseType == ModuleRules.UseTypes.DynamicLibrary)
						CopyAllFilesToFinalPath(IntermediateOutputPath, EnvironmentHelper.DynamicLibraryExtentions);

					State = States.Built;
				}

				if (Module.GenerateRenderDocSettings)
					GenerateRenderDocSettings();
			}
			catch
			{
				ConsoleHelper.WriteError($"Building {Module.Name} has failed");

				State = States.Failed;
			}
		}

		protected override void CreateDirectories()
		{
			base.CreateDirectories();

			if (Module.UseType != ModuleRules.UseTypes.UseOnly)
			{
				if (!Directory.Exists(IntermediateTempPath))
					Directory.CreateDirectory(IntermediateTempPath);

				if (!Directory.Exists(IntermediateGeneratedDirectory))
					Directory.CreateDirectory(IntermediateGeneratedDirectory);
			}
		}

		private bool MustCompile()
		{
			if (Module.UseType == ModuleRules.UseTypes.UseOnly)
				return true;

			if (!File.Exists(OutputTargetName + Module.GetOutputFileExtension()))
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
			string[] files = FileSystemUtilites.GetAllFiles(sourceCodeRootPath, extensions.ToArray());
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

		private void GenerateRenderDocSettings()
		{
			if (Module.UseType != ModuleRules.UseTypes.Executable)
				throw new FrontendException($"Cannot generate RenderDoc settings for {Module.Name} as it is a non-executable module");

			ISerializeObject rootObj = Creator.Create<ISerializeObject>();
			{
				rootObj["rdocCaptureSettings"] = 1;

				ISerializeObject settingsObj = rootObj.AddObject("settings");
				{
					settingsObj["executable"] = OutputTargetName + EnvironmentHelper.ExecutableExtentions;
					settingsObj["workingDir"] = FinalOutputDirectory;

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

		private CPPProject GenerateProject()
		{
			CPPProject cppProj = new CPPProject();
			CPPProject.Profile profile = (CPPProject.Profile)cppProj.CreateProfile();

			profile.AssemblyName = Module.TargetName;
			profile.OutputType = Module.UseType.ToOutputType();
			profile.OutputPath = IntermediateOutputPath;
			profile.MinimalRebuild = false;
			profile.LanguageStandard = CPPProject.Profile.LanguageStandards.CPPLatest;
			profile.PlatformArchitecture = Architecture;
			profile.BuildConfiguration = Configuration;
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

				profile.Optimization = CPPProject.Profile.Optimizations.Disabled;

				profile.GenerateDebugInfo = false;
				profile.RuntimeLibrary = CPPProject.Profile.RuntimeLibraries.MultiThreadedDLL;
			}

			profile.AddIncludeDirectory(FileSystemUtilites.GetParentDirectory(sourceCodeRootPath));

			if (Module.GenerateReflection)
				profile.AddIncludeDirectory(IntermediateGeneratedDirectory);

			string[] dependencies = Module.GetAllDependencies();
			foreach (string dep in dependencies)
				AddDependency(profile, dep);

			profile.AddPreprocessorDefinition(BuildSystemHelper.ExportAPIPreprocessor);
			profile.AddPreprocessorDefinition(Module.GetNamePreprocessor());
			profile.AddPreprocessorDefinition(Module.GetAPIPreprocessor(BuildSystemHelper.APIPreprocessorTypes.Export));
			profile.AddPreprocessorDefinition(Module.GetExternPreprocessor(BuildSystemHelper.ExternPreprocessorTypes.Fill));

			foreach (string def in Module.PreprocessorDefinitions)
				profile.AddPreprocessorDefinition(def);

			profile.AddPreprocessorDefinition(Configuration.GetPreprocessor());
			profile.AddPreprocessorDefinition(EnvironmentHelper.OperatingSystem.GetPreprocessor());
			profile.AddPreprocessorDefinition(Architecture.GetPreprocessor());

			foreach (string lib in Module.DependencyStaticLibraries)
				profile.AddIncludeLibrary(lib);

			string[] files = FileSystemUtilites.GetAllFiles(sourceCodeRootPath, EnvironmentHelper.HeaderFileExtensions);
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

			files = FileSystemUtilites.GetAllFiles(sourceCodeRootPath, EnvironmentHelper.CompileFileExtensions);
			foreach (string file in files)
				cppProj.AddCompileFile(file);

			return cppProj;
		}

		private bool ParseForReflection(string FilePath, string OutputBaseFileName)
		{
			if (reflectionBuildProcess == null)
				reflectionBuildProcess = new ReflectionBuildProcess(Configuration, Architecture);

			return reflectionBuildProcess.Build(FilePath, OutputBaseFileName);
		}

		private void AddDependency(CPPProject.Profile Profile, string Dependency)
		{
			ModuleBuilder dependencyBuilder = targetBuilder.GetModuleBuilder(Dependency);
			ModuleRules dependencyModule = dependencyBuilder.Module;

			Profile.AddIncludeDirectory(FileSystemUtilites.GetParentDirectory(dependencyBuilder.sourceCodeRootPath));

			foreach (string includePath in dependencyModule.IncludePaths)
				Profile.AddIncludeDirectory(dependencyBuilder.sourceCodeRootPath + includePath);

			foreach (string preprocessor in dependencyModule.PreprocessorDefinitions)
				Profile.AddPreprocessorDefinition(preprocessor);

			if (dependencyModule.UseType == ModuleRules.UseTypes.UseOnly)
			{
				foreach (string libraryPath in dependencyModule.LibraryPaths)
					Profile.AddIncludeLibrary(dependencyBuilder.sourceCodeRootPath + libraryPath);
			}
			else
			{
				BuildSystemHelper.APIPreprocessorTypes type = BuildSystemHelper.APIPreprocessorTypes.Empty;
				switch (dependencyModule.UseType)
				{
					case ModuleRules.UseTypes.DynamicLibrary:
						type = BuildSystemHelper.APIPreprocessorTypes.Import;
						break;
				}

				Profile.AddPreprocessorDefinition(dependencyModule.GetAPIPreprocessor(type));
				Profile.AddPreprocessorDefinition(dependencyModule.GetExternPreprocessor(BuildSystemHelper.ExternPreprocessorTypes.Empty));

				string[] libFiles = FileSystemUtilites.GetAllFiles(dependencyBuilder.IntermediateOutputPath, "*" + EnvironmentHelper.StaticLibraryExtentions);

				foreach (string libFile in libFiles)
					Profile.AddIncludeLibrary(libFile);

				if (dependencyModule.GenerateReflection)
					Profile.AddIncludeDirectory(dependencyBuilder.IntermediateGeneratedDirectory);
			}

			foreach (string dep in dependencyModule.PublicDependencyModuleNames)
				AddDependency(Profile, dep);
		}
	}
}