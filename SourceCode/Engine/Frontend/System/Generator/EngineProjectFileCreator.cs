// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Project.Generator;
using Engine.Frontend.System.Build;
using Engine.Frontend.Utilities;
using System.IO;

namespace Engine.Frontend.System.Generator
{
	static class EngineProjectFileCreator
	{
		private static readonly ProjectBase.ProfileBase.BuildConfigurations[] BuildConfigurations = { ProjectBase.ProfileBase.BuildConfigurations.Debug, ProjectBase.ProfileBase.BuildConfigurations.Release };
		private static readonly ProjectBase.ProfileBase.PlatformTypes[] PlatformTypes = { ProjectBase.ProfileBase.PlatformTypes.x86, ProjectBase.ProfileBase.PlatformTypes.x64 };

		private static string WorkingDirectory
		{
			get { return EnvironmentHelper.ExecutingPath + ".." + EnvironmentHelper.PathSeparator + "Engine" + EnvironmentHelper.PathSeparator; }
		}

		private static string ProjectFilePath
		{
			get { return EnvironmentHelper.ExecutingPath + ".." + EnvironmentHelper.PathSeparator + "Engine" + EnvironmentHelper.PathSeparator + "Engine.vcxproj"; }
		}

		public static bool Create()
		{
			RuleLibraryBuilder rulesBuilder = new RuleLibraryBuilder(EnvironmentHelper.ProcessDirectory);
			if (!rulesBuilder.Build())
				return false;

			CPPProject projectFile = new CPPProject();

			string[] files = FileSystemUtilites.GetAllFiles(WorkingDirectory, "*.cs");

			foreach (ProjectBase.ProfileBase.BuildConfigurations configuration in BuildConfigurations)
				foreach (ProjectBase.ProfileBase.PlatformTypes platform in PlatformTypes)
				{
					foreach (BuildRules buildRule in rulesBuilder.Rules)
					{
						foreach (BuildRules.RuleBase rule in buildRule.Rules)
						{
							if (rule.LibraryUseType != BuildRules.LibraryUseTypes.Executable)
								continue;

							CPPProject.Profile profile = (CPPProject.Profile)projectFile.CreateProfile();

							profile.Name = rule.TargetName;

							profile.BuildConfiguration = configuration;
							profile.PlatformType = platform;
							profile.OutputType = ProjectBase.ProfileBase.OutputTypes.Makefile;
							profile.OutputPath = EnvironmentHelper.FinalOutputDirectory + rule.TargetName + EnvironmentHelper.ExecutableExtentions;
							profile.IntermediatePath = EnvironmentHelper.IntermediateDirectory;

							profile.NMakeBuildCommandLine = string.Format("$(SolutionDir)Binaries/Frontend.exe -BuildEngine -{0} -{1}", platform, configuration);
							//profile.NMakeReBuildCommandLine = string.Format("$(SolutionDir)Binaries/Frontend.exe -RebuildEngine -{0} -{1}", platform, configuration);
							//profile.NMakeCleanCommandLine = string.Format("$(SolutionDir)Binaries/Frontend.exe -CleanEngine");

							//profile.AddIncludeDirectories("$(ProjectDir)");

							foreach (BuildRules buildRule1 in rulesBuilder.Rules)
							{
								foreach (BuildRules.RuleBase rule1 in buildRule1.Rules)
								{
									profile.AddIncludeDirectories(FileSystemUtilites.GetParentDirectory(buildRule1.Path));

									if (rule1.IncludesPath != null)
										foreach (string includePath in rule1.IncludesPath)
											profile.AddIncludeDirectories(FileSystemUtilites.PathSeperatorCorrection(buildRule1.Path + includePath));

									profile.AddPreprocessorDefinition(BuildSystemHelper.GetAPIPreprocessor(rule1.TargetName, BuildSystemHelper.APIPreprocessorValues.Empty));

									if (rule1.PreprocessorDefinitions != null)
										foreach (string pd in rule1.PreprocessorDefinitions)
											profile.AddPreprocessorDefinition(pd);
								}
							}

							profile.AddPreprocessorDefinition(BuildSystemHelper.GetConfigurationModePreprocessor(configuration));
							profile.AddPreprocessorDefinition(BuildSystemHelper.GetPlatformPreprocessor(EnvironmentHelper.Platform));
						}
					}
				}

			foreach (string file in files)
				projectFile.AddExtraFile(file);

			files = FileSystemUtilites.GetAllFiles(WorkingDirectory, "*.h");
			foreach (string file in files)
				projectFile.AddIncludeFile(file);

			files = FileSystemUtilites.GetAllFiles(WorkingDirectory, "*.cpp");
			foreach (string file in files)
				projectFile.AddCompileFile(file);

			MicrosoftVCProjectGenerator generator = new MicrosoftVCProjectGenerator();
			generator.ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_0;

			File.WriteAllText(ProjectFilePath, generator.Generate(projectFile));
			File.WriteAllText(ProjectFilePath + ".filters", generator.GenerateFilter(projectFile, WorkingDirectory));

			return true;
		}
	}
}