// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Project.Generator;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using System.IO;

namespace Engine.Frontend.System.Generator
{
	static class EngineProjectFileCreator
	{
		private static string ProjectFilePath
		{
			get { return EnvironmentHelper.EngineDirectory + "Engine.vcxproj"; }
		}

		public static bool Generate()
		{
			RulesLibrary.Instance.Build(false);

			TargetRules[] targets = RulesLibrary.Instance.GetTargetRules(ProjectBase.ProfileBase.BuildConfigurations.Debug, ProjectBase.ProfileBase.PlatformArchitectures.x64);

			CPPProject projectFile = new CPPProject();

			foreach (ProjectBase.ProfileBase.BuildConfigurations configuration in BuildSystemHelper.BuildConfigurations)
				foreach (ProjectBase.ProfileBase.PlatformArchitectures architecture in BuildSystemHelper.PlatformTypes)
					foreach (TargetRules target in targets)
					{
						ModuleRules targetModule = RulesLibrary.Instance.GetModuleRules(target.ModuleName, configuration, architecture);

						CPPProject.Profile profile = (CPPProject.Profile)projectFile.CreateProfile();

						profile.Name = targetModule.Name;
						profile.BuildConfiguration = configuration;
						profile.PlatformArchitecture = architecture;
						profile.OutputType = ProjectBase.ProfileBase.OutputTypes.Makefile;
						profile.OutputPath = EnvironmentHelper.GetOutputDirectory(configuration, architecture) + targetModule.TargetName + EnvironmentHelper.ExecutableExtensions;
						profile.IntermediateDirectory = EnvironmentHelper.IntermediateDirectory;
						profile.LanguageStandard = CPPProject.Profile.LanguageStandards.CPPLatest;

						profile.NMakeBuildCommandLine = $"\"{EnvironmentHelper.FrontenddToolPath}\" -Action {EntryPoint.Actions.BuildEngine} -Target {targetModule.Name} -Architecture {architecture} -Configuration {configuration}";
						profile.NMakeReBuildCommandLine = $"\"{EnvironmentHelper.FrontenddToolPath}\" -Action {EntryPoint.Actions.RebuildEngine} -Target {targetModule.Name} -Architecture {architecture} -Configuration {configuration}";
						profile.NMakeCleanCommandLine = $"\"{EnvironmentHelper.FrontenddToolPath}\" -Action {EntryPoint.Actions.CleanEngine} -Target {targetModule.Name} -Architecture {architecture} -Configuration {configuration}";

						profile.AddPreprocessorDefinition(BuildSystemHelper.ExportAPIPreprocessor);

						ModuleRules[] modules = RulesLibrary.Instance.GetModuleRules(configuration, architecture);
						foreach (ModuleRules module in modules)
						{
							string sourceRootDir = module.GetSourceRootDirectory();
							if (string.IsNullOrEmpty(sourceRootDir))
								continue;

							profile.AddIncludeDirectory(FileSystemUtilites.GetParentDirectory(sourceRootDir));
							profile.AddIncludeDirectory(profile.IntermediateDirectory + module.Name + EnvironmentHelper.PathSeparator + EnvironmentHelper.GeneratedPathName);

							foreach (string includePath in module.IncludePaths)
								profile.AddIncludeDirectory(sourceRootDir + includePath);

							profile.AddPreprocessorDefinition(module.GetAPIPreprocessor(BuildSystemHelper.APIPreprocessorTypes.Empty));
							profile.AddPreprocessorDefinition(module.GetExternPreprocessor(BuildSystemHelper.ExternPreprocessorTypes.Empty));

							foreach (string pd in module.PreprocessorDefinitions)
								profile.AddPreprocessorDefinition(pd);
						}

						profile.AddPreprocessorDefinition(configuration.GetPreprocessor());
						profile.AddPreprocessorDefinition(EnvironmentHelper.OperatingSystem.GetPreprocessor());
						profile.AddPreprocessorDefinition(architecture.GetPreprocessor());
						profile.AddPreprocessorDefinition(BuildSystemHelper.EmptyModuleNamePreprocessor);
						profile.AddPreprocessorDefinition(BuildSystemHelper.IconIDDefinition);
					}

			string[] files = FileSystemUtilites.GetAllFiles(EnvironmentHelper.SourceDirectory, EnvironmentHelper.CSharpFileSearchPattern);
			foreach (string file in files)
				projectFile.AddExtraFile(file);

			files = FileSystemUtilites.GetAllFiles(EnvironmentHelper.SourceDirectory, EnvironmentHelper.DebugVisualizerFileSearchPattern);
			foreach (string file in files)
				projectFile.AddExtraFile(file);

			files = FileSystemUtilites.GetAllFiles(EnvironmentHelper.SourceDirectory, EnvironmentHelper.HeaderFileSearchPattern);
			foreach (string file in files)
				projectFile.AddIncludeFile(file);

			files = FileSystemUtilites.GetAllFiles(EnvironmentHelper.SourceDirectory, EnvironmentHelper.CompileFileSearchPattern);
			foreach (string file in files)
				projectFile.AddCompileFile(file);

			MicrosoftVCProjectGenerator generator = new MicrosoftVCProjectGenerator();
			generator.ToolsVersion = MSBuildProcess.Info.ToolsVersion;

			File.WriteAllText(ProjectFilePath, generator.Generate(projectFile, true));
			File.WriteAllText(ProjectFilePath + ".filters", generator.GenerateFilter(projectFile, EnvironmentHelper.SourceDirectory));

			return true;
		}
	}
}