// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Project.Generator;
using Engine.Frontend.System.Build;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using System.Collections.Generic;
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
			RuleLibraryBuilder rulesBuilder = RuleLibraryBuilder.Instance;
			rulesBuilder.Initialize();

			List<ModuleRules> modules = new List<ModuleRules>();
			List<TargetRules> targets = new List<TargetRules>();

			NewModuleRuleEventHandler newModuleCallback = (rule) =>
			{
				modules.Add(rule);
			};

			NewTargetRuleEventHandler newTargetCallback = (rule) =>
			{
				targets.Add(rule);
			};

			rulesBuilder.OnNewModuleRule += newModuleCallback;
			rulesBuilder.OnNewTargetRule += newTargetCallback;

			rulesBuilder.Build(false);

			CPPProject projectFile = new CPPProject();

			foreach (ProjectBase.ProfileBase.BuildConfigurations configuration in BuildSystemHelper.BuildConfigurations)
				foreach (ProjectBase.ProfileBase.PlatformArchitectures platform in BuildSystemHelper.PlatformTypes)
					foreach (TargetRules target in targets)
					{
						ModuleRules targetModule = target.GetModule();

						foreach (ModuleRules.BuildRulesBase targetBuild in targetModule.BuildRules)
						{
							CPPProject.Profile profile = (CPPProject.Profile)projectFile.CreateProfile();

							profile.Name = targetModule.Name;
							profile.BuildConfiguration = configuration;
							profile.PlatformArchitecture = platform;
							profile.OutputType = ProjectBase.ProfileBase.OutputTypes.Makefile;
							profile.OutputPath = BuildSystemHelper.GetOutputDirectory(configuration, platform) + targetBuild.TargetName + EnvironmentHelper.ExecutableExtentions;
							profile.IntermediateDirectory = EnvironmentHelper.IntermediateDirectory;
							profile.LanguageStandard = CPPProject.Profile.LanguageStandards.CPPLatest;

							profile.NMakeBuildCommandLine = $"\"{EnvironmentHelper.FrontenddToolPath}\" -Action {EntryPoint.Actions.BuildEngine} -Architecture {platform} -Configuration {configuration}";
							profile.NMakeReBuildCommandLine = $"\"{EnvironmentHelper.FrontenddToolPath}\" -Action {EntryPoint.Actions.RebuildEngine} -Architecture {platform} -Configuration {configuration}";
							profile.NMakeCleanCommandLine = $"\"{EnvironmentHelper.FrontenddToolPath}\" -Action {EntryPoint.Actions.CleanEngine} -Architecture {platform} -Configuration {configuration}";

							profile.AddPreprocessorDefinition(BuildSystemHelper.GetExportAPIPreprocessorRaw());

							foreach (ModuleRules module in modules)
								foreach (ModuleRules.BuildRulesBase build in module.BuildRules)
								{
									string sourceRootDir = module.GetSourceRootDirectory();
									if (string.IsNullOrEmpty(sourceRootDir))
										continue;

									profile.AddIncludeDirectory(FileSystemUtilites.GetParentDirectory(sourceRootDir));
									profile.AddIncludeDirectory(FileSystemUtilites.PathSeperatorCorrection(profile.IntermediateDirectory + module.Name + EnvironmentHelper.PathSeparator + EnvironmentHelper.GeneratedPathName));

									if (build.IncludePaths != null)
										foreach (string includePath in build.IncludePaths)
											profile.AddIncludeDirectory(FileSystemUtilites.PathSeperatorCorrection(sourceRootDir + includePath));

									profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(build.TargetName, BuildSystemHelper.APIPreprocessorTypes.Empty));
									profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(build.TargetName, BuildSystemHelper.ExternPreprocessorTypes.Empty));

									if (build.PreprocessorDefinitions != null)
										foreach (string pd in build.PreprocessorDefinitions)
											profile.AddPreprocessorDefinition(pd);
								}

							profile.AddPreprocessorDefinition(BuildSystemHelper.GetConfigurationModePreprocessor(configuration));
							profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformPreprocessor(EnvironmentHelper.Platform));
							profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformTypesPreprocessor(platform));
							profile.AddPreprocessorDefinition(BuildSystemHelper.GetModuleNamePreprocessor(""));
						}
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

			rulesBuilder.OnNewModuleRule -= newModuleCallback;
			rulesBuilder.OnNewTargetRule -= newTargetCallback;

			return true;
		}
	}
}