// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Project.Generator;
using Engine.Frontend.System.Build;
using Engine.Frontend.System.Compile;
using Engine.Frontend.Utilities;
using System;
using System.Collections.Generic;
using System.IO;

namespace Engine.Frontend.System.Generator
{
	static class EngineProjectFileCreator
	{
		private static string WorkingDirectory
		{
			get { return EnvironmentHelper.ExecutingPath + ".." + EnvironmentHelper.PathSeparator + "Engine" + EnvironmentHelper.PathSeparator; }
		}

		private static string ProjectFilePath
		{
			get { return EnvironmentHelper.ExecutingPath + ".." + EnvironmentHelper.PathSeparator + "Engine" + EnvironmentHelper.PathSeparator + "Engine.vcxproj"; }
		}

		public static bool Generate()
		{
			RuleLibraryBuilder rulesBuilder = RuleLibraryBuilder.Instance;

			List<BuildRules> rules = new List<BuildRules>();

			NewBuildRuleEventHandler newRuleCallback = (filePath, rule) =>
			{
				rules.Add(rule);
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
					foreach (BuildRules buildRule in rules)
						foreach (BuildRules.RuleBase rule in buildRule.Rules)
						{
							if (rule.LibraryUseType != BuildRules.LibraryUseTypes.Executable)
								continue;

							CPPProject.Profile profile = (CPPProject.Profile)projectFile.CreateProfile();

							profile.Name = buildRule.ModuleName;
							profile.BuildConfiguration = configuration;
							profile.PlatformArchitecture = platform;
							profile.OutputType = ProjectBase.ProfileBase.OutputTypes.Makefile;
							profile.OutputPath = EnvironmentHelper.FinalOutputDirectory + rule.TargetName + EnvironmentHelper.ExecutableExtentions;
							profile.IntermediatePath = EnvironmentHelper.IntermediateDirectory;

							profile.NMakeBuildCommandLine = string.Format("\"$(SolutionDir)Binaries/Frontend.exe\" -Action BuildEngine -Architecture {0} -Configuration {1}", platform, configuration);
							profile.NMakeReBuildCommandLine = string.Format("\"$(SolutionDir)Binaries/Frontend.exe\" -Action RebuildEngine -Architecture {0} -Configuration {1}", platform, configuration);
							profile.NMakeCleanCommandLine = string.Format("\"$(SolutionDir)Binaries/Frontend.exe\" -Action CleanEngine -Architecture {0} -Configuration {1}", platform, configuration);

							//profile.AddIncludeDirectories("$(ProjectDir)");

							foreach (BuildRules buildRule1 in rules)
							{
								foreach (BuildRules.RuleBase rule1 in buildRule1.Rules)
								{
									profile.AddIncludeDirectories(FileSystemUtilites.GetParentDirectory(buildRule1.Path));
									profile.AddIncludeDirectories(FileSystemUtilites.PathSeperatorCorrection(buildRule1.Path));
									profile.AddIncludeDirectories(FileSystemUtilites.PathSeperatorCorrection(profile.IntermediatePath + rule1.TargetName + EnvironmentHelper.PathSeparator + BuildSystemHelper.GeneratedPathName));

									if (rule1.IncludesPath != null)
										foreach (string includePath in rule1.IncludesPath)
											profile.AddIncludeDirectories(FileSystemUtilites.PathSeperatorCorrection(buildRule1.Path + includePath));

									profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(rule1.TargetName, BuildSystemHelper.APIPreprocessorTypes.Empty));
									profile.AddPreprocessorDefinition(BuildSystemHelper.GetExternPreprocessor(rule1.TargetName, BuildSystemHelper.ExternPreprocessorTypes.Empty));

									if (rule1.PreprocessorDefinitions != null)
										foreach (string pd in rule1.PreprocessorDefinitions)
											profile.AddPreprocessorDefinition(pd);
								}
							}

							profile.AddPreprocessorDefinition(BuildSystemHelper.GetConfigurationModePreprocessor(configuration));
							profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformPreprocessor(EnvironmentHelper.Platform));
							profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformTypesPreprocessor(platform));
							profile.AddPreprocessorDefinition(BuildSystemHelper.GetModuleNamePreprocessor(""));
						}

			string[] files = FileSystemUtilites.GetAllFiles(WorkingDirectory, EnvironmentHelper.CSharpFileExtensions);

			foreach (string file in files)
				projectFile.AddExtraFile(file);

			files = FileSystemUtilites.GetAllFiles(WorkingDirectory, EnvironmentHelper.HeaderFileExtensions);
			foreach (string file in files)
				projectFile.AddIncludeFile(file);

			files = FileSystemUtilites.GetAllFiles(WorkingDirectory, EnvironmentHelper.CompileFileExtensions);
			foreach (string file in files)
				projectFile.AddCompileFile(file);

			MicrosoftVCProjectGenerator generator = new MicrosoftVCProjectGenerator();
			generator.ToolsVersion = MSBuildProcess.Info.ToolsVersion;

			File.WriteAllText(ProjectFilePath, generator.Generate(projectFile, true));
			File.WriteAllText(ProjectFilePath + ".filters", generator.GenerateFilter(projectFile, WorkingDirectory));

			return true;
		}
	}
}