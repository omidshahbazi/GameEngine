// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.ProjectFileGenerator;
using Engine.Frontend.Utilities;
using System.IO;

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

			VCProjectFile projectFile = new VCProjectFile();


			File.WriteAllText(ProjectFilePath, projectFile.Content);
		}
	}
}