// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Project.Generator;
using Engine.Frontend.Utilities;
using System.IO;

namespace Engine.Frontend.System
{
	static class EngineProjectFileCreator
	{
		private static readonly ProjectBase.ProfileBase.BuildConfigurations[] BUILD_CONFIGURATION = { ProjectBase.ProfileBase.BuildConfigurations.Debug, ProjectBase.ProfileBase.BuildConfigurations.Release };
		private static readonly ProjectBase.ProfileBase.PlatformTypes[] PLATFORM_TYPES = { ProjectBase.ProfileBase.PlatformTypes.x86, ProjectBase.ProfileBase.PlatformTypes.x64 };

		private static string WorkingDirectory
		{
			get { return EnvironmentHelper.ExecutingPath + ".." + EnvironmentHelper.PathSeparator + "Engine" + EnvironmentHelper.PathSeparator; }
		}

		private static string ProjectFilePath
		{
			//get { return EnvironmentHelper.ExecutingPath + ".." + EnvironmentHelper.PathSeparator + "Engine" + EnvironmentHelper.PathSeparator + "Engine.vcxproj"; }
			get { return EnvironmentHelper.ExecutingPath + ".." + EnvironmentHelper.PathSeparator + "Test" + EnvironmentHelper.PathSeparator + "Test.vcxproj"; }
		}

		public static void Create()
		{
			CPPProject projectFile = new CPPProject();

			for (int i = 0; i < BUILD_CONFIGURATION.Length; ++i)
				for (int j = 0; j < PLATFORM_TYPES.Length; ++j)
				{
					CPPProject.Profile profile = (CPPProject.Profile)projectFile.CreateProfile();

					profile.BuildConfiguration = BUILD_CONFIGURATION[i];
					profile.PlatformType = PLATFORM_TYPES[j];
					profile.OutputType = ProjectBase.ProfileBase.OutputTypes.Makefile;
					profile.OutputPath = EnvironmentHelper.ExecutingPath + "TestNetwork.exe";
					profile.OutputPath = EnvironmentHelper.ExecutingPath + "TestNetwork.exe";
				}

			MicrosoftVCProjectGenerator generator = new MicrosoftVCProjectGenerator();
			generator.ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_0;

			string[] files = FileSystemUtilites.GetAllFiles(WorkingDirectory, "*.h");
			for (int i = 0; i < files.Length; ++i)
				projectFile.AddIncludeFile(files[i]);

			files = FileSystemUtilites.GetAllFiles(WorkingDirectory, "*.cpp");
			for (int i = 0; i < files.Length; ++i)
				projectFile.AddCompileFile(files[i]);

			files = FileSystemUtilites.GetAllFiles(WorkingDirectory, "*.cs");
			for (int i = 0; i < files.Length; ++i)
				projectFile.AddExtraFile(files[i]);

			File.WriteAllText(ProjectFilePath, generator.Generate(projectFile));
		}
	}
}