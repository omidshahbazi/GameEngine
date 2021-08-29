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

			NewBuildRuleEventHandler newRuleCallback = (filePath, rule) =>
			{
				modules.Add(rule);
			};

			rulesBuilder.OnNewBuildRule += newRuleCallback;

			if (!rulesBuilder.Build(false))
			{
				rulesBuilder.OnNewBuildRule -= newRuleCallback;
				return false;
			}

			rulesBuilder.OnNewBuildRule -= newRuleCallback;

			CPPProject projectFile = new CPPProject();

			foreach (ProjectBase.ProfileBase.BuildConfigurations configuration in BuildSystemHelper.BuildConfigurations)
				foreach (ProjectBase.ProfileBase.PlatformArchitectures platform in BuildSystemHelper.PlatformTypes)
					foreach (ModuleRules module1 in modules)
						foreach (ModuleRules.BuildRulesBase build1 in module1.BuildRules)
						{
							if (build1.LibraryUseType != ModuleRules.LibraryUseTypes.Executable)
								continue;

							CPPProject.Profile profile = (CPPProject.Profile)projectFile.CreateProfile();

							profile.Name = module1.Name;
							profile.BuildConfiguration = configuration;
							profile.PlatformArchitecture = platform;
							profile.OutputType = ProjectBase.ProfileBase.OutputTypes.Makefile;
							profile.OutputPath = BuildSystemHelper.GetOutputDirectory(configuration, platform) + build1.TargetName + EnvironmentHelper.ExecutableExtentions;
							profile.IntermediatePath = EnvironmentHelper.IntermediateDirectory;
							profile.LanguageStandard = CPPProject.Profile.LanguageStandards.CPPLatest;

							profile.NMakeBuildCommandLine = $"\"{EnvironmentHelper.FrontenddToolPath}\" -Action BuildEngine -Architecture {platform} -Configuration {configuration}";
							profile.NMakeReBuildCommandLine = $"\"{EnvironmentHelper.FrontenddToolPath}\" -Action RebuildEngine -Architecture {platform} -Configuration {configuration}";
							profile.NMakeCleanCommandLine = $"\"{EnvironmentHelper.FrontenddToolPath}\" -Action CleanEngine -Architecture {platform} -Configuration {configuration}";

							//profile.AddIncludeDirectories("$(ProjectDir)");

							foreach (ModuleRules module2 in modules)
								foreach (ModuleRules.BuildRulesBase build2 in module2.BuildRules)
								{
									profile.AddIncludeDirectory(FileSystemUtilites.GetParentDirectory(module2.Path));
									profile.AddIncludeDirectory(FileSystemUtilites.PathSeperatorCorrection(profile.IntermediatePath + module2.Name + EnvironmentHelper.PathSeparator + EnvironmentHelper.GeneratedPathName));

									if (build2.IncludePaths != null)
										foreach (string includePath in build2.IncludePaths)
											profile.AddIncludeDirectory(FileSystemUtilites.PathSeperatorCorrection(module2.Path + includePath));

									profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(build2.TargetName, BuildSystemHelper.APIPreprocessorTypes.Empty));
									profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(build2.TargetName, BuildSystemHelper.ExternPreprocessorTypes.Empty));

									if (build2.PreprocessorDefinitions != null)
										foreach (string pd in build2.PreprocessorDefinitions)
											profile.AddPreprocessorDefinition(pd);
								}

							profile.AddPreprocessorDefinition(BuildSystemHelper.GetConfigurationModePreprocessor(configuration));
							profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformPreprocessor(EnvironmentHelper.Platform));
							profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformTypesPreprocessor(platform));
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