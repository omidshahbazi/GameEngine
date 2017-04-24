﻿// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.ProjectFile;
using Engine.Frontend.Utilities;

namespace Engine.Frontend.System
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

		public static void Create()
		{
			string[] files = FileSystemUtilites.GetAllFiles(WorkingDirectory, "*.h", "*.cpp", "*.cs");

			VCProjectFileGenerator projectFile = new VCProjectFileGenerator();


			projectFile.Generate(ProjectFilePath);
		}
	}
}