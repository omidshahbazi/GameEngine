// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Project.Generator;
using System.IO;
using System;
using Microsoft.Win32;

namespace Engine.Frontend.System.Compile
{
	class MSBuildProcess : BuildProcess
	{
		public static class Info
		{
			public static string Path
			{
				get;
				private set;
			}

			public static MicrosoftVCProjectGenerator.ToolsVersions ToolsVersion
			{
				get;
				private set;
			}

			static Info()
			{
				Initialize();
			}

			private static void Initialize()
			{
				try
				{
					Fetch_FirstMethod();

					return;
				}
				catch { }

				Fetch_SecondMethod();
			}

			private static void Fetch_FirstMethod()
			{
				const string PATH = @"SOFTWARE\WOW6432Node\Microsoft\VisualStudio\SxS\VS7\";

				RegistryKey registry = Registry.LocalMachine.OpenSubKey(PATH);

				string[] versions = registry.GetValueNames();

				int largest = 0;
				foreach (string version in versions)
				{
					float ver = 0.0F;
					if (!float.TryParse(version, out ver))
						continue;

					if (ver > largest)
						largest = (int)ver;
				}

				const string NOT_FOUND_EXCEPTION_TEXT = "There isn't any Microsoft Build Tool installed on the machine";

				if (largest == 0.0F)
					throw new Exception(NOT_FOUND_EXCEPTION_TEXT);

				string versionStr = largest.ToString("F1");

				string[] files = Directory.GetFiles(registry.GetValue(versionStr).ToString(), "MSBuild.exe", SearchOption.AllDirectories);

				if (files.Length == 0)
					throw new Exception(NOT_FOUND_EXCEPTION_TEXT);

				Path = files[0];

				switch (largest)
				{
					case 14:
						ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_0;
						break;

					case 15:
						ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_2;
						break;

					default:
						throw new Exception(NOT_FOUND_EXCEPTION_TEXT);
				}
			}

			private static void Fetch_SecondMethod()
			{
				const string PATH = @"SOFTWARE\Microsoft\MSBuild\ToolsVersions\";

				RegistryKey registry = Registry.LocalMachine.OpenSubKey(PATH);

				string[] versions = registry.GetSubKeyNames();

				int largest = 0;
				foreach (string version in versions)
				{
					float ver = 0.0F;
					if (!float.TryParse(version, out ver))
						continue;

					if (ver > largest)
						largest = (int)ver;
				}

				const string NOT_FOUND_EXCEPTION_TEXT = "There isn't any Microsoft Build Tool installed on the machine";

				if (largest == 0.0F)
					throw new Exception(NOT_FOUND_EXCEPTION_TEXT);

				string versionStr = largest.ToString("F1");

				registry = Registry.LocalMachine.OpenSubKey(PATH + versionStr + @"\");

				string[] files = Directory.GetFiles(registry.GetValue("MSBuildToolsPath").ToString(), "MSBuild.exe");

				if (files.Length == 0)
					throw new Exception(NOT_FOUND_EXCEPTION_TEXT);

				Path = files[0];

				switch (largest)
				{
					case 14:
						ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_0;
						break;

					case 4:
						ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_2;
						break;

					default:
						throw new Exception(NOT_FOUND_EXCEPTION_TEXT);
				}
			}
		}

		public MSBuildProcess()
		{
			FilePath = Info.Path;
		}

		public override void Build(string ProjectPath, ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration, ProjectBase.ProfileBase.PlatformTypes PlatformType)
		{
			Start(string.Format("\"{0}\" /t:build /p:configuration={1} /p:platform={2}", ProjectPath, BuildConfiguration.ToString().ToLower(), PlatformType.ToString()));
		}

		public override void Build(ProjectBase.ProfileBase ProjectProfile)
		{
			string projPath = ProjectProfile.IntermediatePath + ProjectProfile.AssemblyName;

			MSBuildProjectGenerator projectGenerator = null;

			if (ProjectProfile is CPPProject.Profile)
			{
				MicrosoftVCProjectGenerator generator = new MicrosoftVCProjectGenerator();
				projectGenerator = generator;
				generator.ToolsVersion = Info.ToolsVersion;

				projPath += ".vcxproj";
			}
			else if (ProjectProfile is CSProject.Profile)
			{
				projectGenerator = new MicrosoftCSProjectGenerator();

				projPath += ".csproj";
			}

			File.WriteAllText(projPath, projectGenerator.Generate(ProjectProfile.Project, false));

			Build(projPath, ProjectProfile.BuildConfiguration, ProjectProfile.PlatformType);
		}

		public override void Rebuild(string ProjectPath, ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration, ProjectBase.ProfileBase.PlatformTypes PlatformType)
		{
			Start(string.Format("\"{0}\" /t:clean;build /p:configuration={1} /p:platform={2}", ProjectPath, BuildConfiguration.ToString().ToLower(), PlatformType.ToString()));
		}

		public override void Clean(string ProjectPath)
		{
			Start(string.Format("\"{0}\" /t:clean", ProjectPath));
		}
	}
}