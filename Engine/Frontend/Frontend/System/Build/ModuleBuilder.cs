// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.System.Compile;
using Engine.Frontend.System.Generator;
using Engine.Frontend.Utilities;
using GameFramework.ASCIISerializer;
using GameFramework.Common.Utilities;
using System.Collections.Generic;
using System.IO;

namespace Engine.Frontend.System.Build
{
	class ModuleBuilder : BaseBuilder
	{
		private const string KEY_FRONTENT_HASH = "FH";
		private const string KEY_REFLECTION_TOOL_HASH = "RTH";

		private TargetBuilder targetBuilder = null;
		private string sourceCodeRootPath = "";

		protected static ReflectionBuildProcess reflectionBuildProcess = null;

		private string HashesFilePath
		{
			get { return $"{IntermediateTempPath}Hashes.json"; }
		}

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
				ConsoleHelper.WriteInfo($"	{ModuleName} is Already Up-to-Date");

				State = States.AlreadyUpdated;
				return;
			}

			ConsoleHelper.WriteInfo($"	{ModuleName} is Going to Build");

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

					State = States.Built;
				}
				else
				{
					FileSystemUtilites.DeleteAllSubs(IntermediateGeneratedDirectory);

					CPPProject cppProject = GenerateProject();

					Compile(cppProject.Profiles[0]);

					if (Module.UseType == ModuleRules.UseTypes.Executable)
						CopyAllFilesToFinalPath(IntermediateOutputPath, EnvironmentHelper.ExecutableExtensions);
					else if (Module.UseType == ModuleRules.UseTypes.DynamicLibrary)
						CopyAllFilesToFinalPath(IntermediateOutputPath, EnvironmentHelper.DynamicLibraryExtensions);

					State = States.Built;

					if (Module.GenerateRenderDocSettings)
						GenerateRenderDocSettings();
				}
			}
			catch (FrontendException ex)
			{
				ConsoleHelper.WriteError($"Building {Module.Name} has failed details: ({ex.Message})");

				State = States.Failed;
			}
			catch
			{
				ConsoleHelper.WriteError($"Building {Module.Name} has failed");

				State = States.Failed;
			}

			GenerateHashes(State == States.Failed);
		}

		protected override void CreateDirectories()
		{
			base.CreateDirectories();

			if (!Directory.Exists(IntermediateTempPath))
				Directory.CreateDirectory(IntermediateTempPath);

			if (Module.UseType != ModuleRules.UseTypes.UseOnly)
			{
				if (!Directory.Exists(IntermediateGeneratedDirectory))
					Directory.CreateDirectory(IntermediateGeneratedDirectory);
			}
		}

		private bool MustCompile()
		{
			if (Module.UseType != ModuleRules.UseTypes.UseOnly)
			{
				if (!File.Exists(OutputTargetName + Module.GetOutputFileExtension()))
					if (FileSystemUtilites.GetAllFiles(sourceCodeRootPath, EnvironmentHelper.CompileFileSearchPattern).Length != 0)
						return true;

				foreach (string moduleName in Module.GetAllDependencies())
				{
					if (targetBuilder.GetModuleBuilder(moduleName).State == States.Built)
						return true;
				}
			}

			if (!File.Exists(HashesFilePath))
				return true;

			ISerializeObject hashesData = Creator.Create<ISerializeObject>(File.ReadAllText(HashesFilePath));

			if (hashesData.Get<uint>(KEY_FRONTENT_HASH) != EnvironmentHelper.FrontenddToolHash)
				return true;

			if (Module.GenerateReflection && hashesData.Get<uint>(KEY_REFLECTION_TOOL_HASH) != EnvironmentHelper.GetReflectionToolHash(Configuration, Architecture))
				return true;

			List<string> extensions = new List<string>();
			extensions.AddRange(EnvironmentHelper.HeaderFileSearchPattern);

			if (Module.UseType == ModuleRules.UseTypes.UseOnly)
			{
				extensions.Add($"*{EnvironmentHelper.StaticLibraryExtensions}");
				extensions.Add($"*{EnvironmentHelper.DynamicLibraryExtensions}");
			}
			else
			{
				extensions.AddRange(EnvironmentHelper.CompileFileSearchPattern);
				extensions.AddRange(EnvironmentHelper.CSharpFileSearchPattern);
			}

			string[] files = FileSystemUtilites.GetAllFiles(sourceCodeRootPath, extensions.ToArray());
			foreach (string file in files)
			{
				string filePathHash = GetHash(file).ToString();
				int contentHash = GetHash(File.ReadAllText(file));

				if (hashesData.Contains(filePathHash) && hashesData.Get<int>(filePathHash) == contentHash)
					continue;

				return true;
			}

			return false;
		}

		private void GenerateHashes(bool Failed)
		{
			ISerializeObject hashesData = Creator.Create<ISerializeObject>();

			if (!Failed)
			{
				hashesData[KEY_FRONTENT_HASH] = EnvironmentHelper.FrontenddToolHash;

				if (Module.GenerateReflection)
					hashesData[KEY_REFLECTION_TOOL_HASH] = EnvironmentHelper.GetReflectionToolHash(Configuration, Architecture);

				List<string> extensions = new List<string>();
				extensions.AddRange(EnvironmentHelper.HeaderFileSearchPattern);

				if (Module.UseType == ModuleRules.UseTypes.UseOnly)
				{
					extensions.AddRange(EnvironmentHelper.HeaderFileSearchPattern);
					extensions.Add($"*{EnvironmentHelper.StaticLibraryExtensions}");
					extensions.Add($"*{EnvironmentHelper.DynamicLibraryExtensions}");
				}
				else
				{
					extensions.AddRange(EnvironmentHelper.CompileFileSearchPattern);
					extensions.AddRange(EnvironmentHelper.CSharpFileSearchPattern);
				}

				string[] files = FileSystemUtilites.GetAllFiles(sourceCodeRootPath, extensions.ToArray());

				foreach (string file in files)
					hashesData.Set(GetHash(file).ToString(), GetHash(File.ReadAllText(file)));
			}

			File.WriteAllText(HashesFilePath, hashesData.Content);
		}

		private void GenerateRenderDocSettings()
		{
			if (Module.UseType != ModuleRules.UseTypes.Executable)
				throw new FrontendException($"Cannot generate RenderDoc settings for {Module.Name} as it is a non-executable module");

			RenderDocSettingsGenerator.Generate(OutputTargetName + ".RenderDoc.cap", OutputTargetName + EnvironmentHelper.ExecutableExtensions, FinalOutputDirectory, "DeviceType", string.Empty);
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

			string[] files = FileSystemUtilites.GetAllFiles(sourceCodeRootPath, EnvironmentHelper.HeaderFileSearchPattern);
			foreach (string file in files)
				cppProj.AddIncludeFile(file);

			if (Module.GenerateReflection)
				foreach (string file in files)
				{
					string outputBaseFileName = IntermediateGeneratedDirectory + Path.GetFileNameWithoutExtension(file) + ".Reflection";
					if (ParseForReflection(file, outputBaseFileName))
					{
						cppProj.AddIncludeFile(outputBaseFileName + EnvironmentHelper.HeaderFileExtension);
						cppProj.AddCompileFile(outputBaseFileName + EnvironmentHelper.CompileFileExtension);
					}
				}

			files = FileSystemUtilites.GetAllFiles(sourceCodeRootPath, EnvironmentHelper.CompileFileSearchPattern);
			foreach (string file in files)
				cppProj.AddCompileFile(file);

			profile.AddPreprocessorDefinition(BuildSystemHelper.IconIDDefinition);
			cppProj.ResourceDefinition = new ResourceDefinition();
			cppProj.ResourceDefinition.ProductName = Module.Name;
			cppProj.ResourceDefinition.ProductVersion = EnvironmentHelper.Version;
			cppProj.ResourceDefinition.Copyright = EnvironmentHelper.Copyright;

			if (Module.UseType == ModuleRules.UseTypes.Executable)
				cppProj.ResourceDefinition.IconPath = EnvironmentHelper.WorkingDirectory + Module.IconPath;

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

				string[] libFiles = FileSystemUtilites.GetAllFiles(dependencyBuilder.IntermediateOutputPath, $"*{EnvironmentHelper.StaticLibraryExtensions}");

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