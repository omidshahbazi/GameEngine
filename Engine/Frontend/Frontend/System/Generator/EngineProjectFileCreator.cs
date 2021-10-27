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

			TargetRules[] targets = RulesLibrary.Instance.TargetRules;

			CPPProject projectFile = new CPPProject();

			foreach (ProjectBase.ProfileBase.BuildConfigurations configuration in BuildSystemHelper.BuildConfigurations)
				foreach (ProjectBase.ProfileBase.PlatformArchitectures platform in BuildSystemHelper.PlatformTypes)
					foreach (TargetRules target in targets)
					{
						ModuleRules targetModule = target.GetModule(configuration, platform);

						CPPProject.Profile profile = (CPPProject.Profile)projectFile.CreateProfile();

						profile.Name = targetModule.Name;
						profile.BuildConfiguration = configuration;
						profile.PlatformArchitecture = platform;
						profile.OutputType = ProjectBase.ProfileBase.OutputTypes.Makefile;
						profile.OutputPath = BuildSystemHelper.GetOutputDirectory(configuration, platform) + targetModule.TargetName + EnvironmentHelper.ExecutableExtentions;
						profile.IntermediateDirectory = EnvironmentHelper.IntermediateDirectory;
						profile.LanguageStandard = CPPProject.Profile.LanguageStandards.CPPLatest;

						profile.NMakeBuildCommandLine = $"\"{EnvironmentHelper.FrontenddToolPath}\" -Action {EntryPoint.Actions.BuildEngine} -Architecture {platform} -Configuration {configuration}";
						profile.NMakeReBuildCommandLine = $"\"{EnvironmentHelper.FrontenddToolPath}\" -Action {EntryPoint.Actions.RebuildEngine} -Architecture {platform} -Configuration {configuration}";
						profile.NMakeCleanCommandLine = $"\"{EnvironmentHelper.FrontenddToolPath}\" -Action {EntryPoint.Actions.CleanEngine} -Architecture {platform} -Configuration {configuration}";

						profile.AddPreprocessorDefinition(BuildSystemHelper.GetExportAPIPreprocessorRaw());

						ModuleRules[] modules = RulesLibrary.Instance.GetModuleRules(configuration, platform);
						foreach (ModuleRules module in modules)
						{
							string sourceRootDir = module.GetSourceRootDirectory();
							if (string.IsNullOrEmpty(sourceRootDir))
								continue;

							profile.AddIncludeDirectory(FileSystemUtilites.GetParentDirectory(sourceRootDir));
							profile.AddIncludeDirectory(FileSystemUtilites.PathSeperatorCorrection(profile.IntermediateDirectory + module.Name + EnvironmentHelper.PathSeparator + EnvironmentHelper.GeneratedPathName));

							foreach (string includePath in module.IncludePaths)
								profile.AddIncludeDirectory(FileSystemUtilites.PathSeperatorCorrection(sourceRootDir + includePath));

							profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(module.TargetName, BuildSystemHelper.APIPreprocessorTypes.Empty));
							profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(module.TargetName, BuildSystemHelper.ExternPreprocessorTypes.Empty));

							foreach (string pd in module.PreprocessorDefinitions)
								profile.AddPreprocessorDefinition(pd);
						}

						profile.AddPreprocessorDefinition(BuildSystemHelper.GetConfigurationModePreprocessor(configuration));
						profile.AddPreprocessorDefinition(BuildSystemHelper.GetOperatingSystemPreprocessor(EnvironmentHelper.OperatingSystem));
						profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformArchitecturePreprocessor(platform));
						profile.AddPreprocessorDefinition(BuildSystemHelper.GetModuleNamePreprocessor(""));
					}

			string[] files = FileSystemUtilites.GetAllFiles(EnvironmentHelper.SourceDirectory, EnvironmentHelper.CSharpFileExtensions);

			foreach (string file in files)
				projectFile.AddExtraFile(file);

			files = FileSystemUtilites.GetAllFiles(EnvironmentHelper.SourceDirectory, EnvironmentHelper.HeaderFileExtensions);
			foreach (string file in files)
				projectFile.AddIncludeFile(file);

			files = FileSystemUtilites.GetAllFiles(EnvironmentHelper.SourceDirectory, EnvironmentHelper.CompileFileExtensions);
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