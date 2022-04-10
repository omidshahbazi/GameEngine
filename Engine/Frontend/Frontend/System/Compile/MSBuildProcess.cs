// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Project.Generator;
using System.IO;
using System;
using Microsoft.Win32;
using System.Text.RegularExpressions;
using Engine.Frontend.Utilities;
using GameFramework.Common.Utilities;

namespace Engine.Frontend.System.Compile
{
	class MSBuildProcess : BuildProcess
	{
		public static class Info
		{
			private const string NOT_FOUND_EXCEPTION_TEXT = "There isn't any Microsoft Build Tool installed on the machine";

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

			//https://docs.microsoft.com/en-us/visualstudio/install/tools-for-managing-visual-studio-instances?view=vs-2019
			private static void Initialize()
			{
				try
				{
					Fetch_Method1();

					return;
				}
				catch { }

				try
				{
					Fetch_Method2();

					return;
				}
				catch { }

				Fetch_Method3();
			}

			private static void Fetch_Method1()
			{
				//Visual Studio 6 - 6.0
				//Visual Studio .NET(2002) - 7.0
				//Visual Studio .NET 2003 - 7.1
				//Visual Studio 2005 - 8.0
				//Visual Studio 2008 - 9.0
				//Visual Studio 2010 - 10.0
				//Visual Studio 2012 - 11.0
				//Visual Studio 2013 - 12.0
				//Visual Studio 2015 - 14.0
				//Visual Studio 2017 - 15.0
				//Visual Studio 2019 - 16.0
				//Visual Studio 2022 - 17.0

				const string VS_WHERE_PATH = "C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe";

				if (!File.Exists(VS_WHERE_PATH))
					throw new FrontendException(NOT_FOUND_EXCEPTION_TEXT);

				CommandLineProcess process = new CommandLineProcess();
				process.FilePath = VS_WHERE_PATH;
				process.Start();
				string output = process.Output.ReadToEnd();

				const string PATH_KEY = "installationPath: ";
				int index = output.IndexOf(PATH_KEY);
				index += PATH_KEY.Length;
				int endOfLineIndex = output.IndexOf('\r', index);
				string path = output.Substring(index, endOfLineIndex - index);

				string[] files = Directory.GetFiles(path, "MSBuild.exe", SearchOption.AllDirectories);
				if (files.Length == 0)
					throw new FrontendException(NOT_FOUND_EXCEPTION_TEXT);
				Path = files[0];

				const string VERSION_KEY = "installationVersion: ";
				index = output.IndexOf(VERSION_KEY);
				index += VERSION_KEY.Length;
				int firstPointIndex = output.IndexOf('.', index);

				int version = Convert.ToInt32(output.Substring(index, firstPointIndex - index));

				switch (version)
				{
					case 14:
						ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_0;
						break;

					case 15:
						ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_1;
						break;

					case 16:
						ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_2;
						break;

					case 17:
						ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_3;
						break;

					default:
						throw new FrontendException(NOT_FOUND_EXCEPTION_TEXT);
				}
			}

			private static void Fetch_Method2()
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

				if (largest == 0.0F)
					throw new FrontendException(NOT_FOUND_EXCEPTION_TEXT);

				string versionStr = largest.ToString("F1");

				registry = Registry.LocalMachine.OpenSubKey(PATH + versionStr + @"\");

				string[] files = Directory.GetFiles(registry.GetValue("MSBuildToolsPath").ToString(), "MSBuild.exe");
				if (files.Length == 0)
					throw new FrontendException(NOT_FOUND_EXCEPTION_TEXT);
				Path = files[0];

				switch (largest)
				{
					case 14:
						ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_0;
						break;

					case 4:
						ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_1;
						break;

					default:
						throw new FrontendException(NOT_FOUND_EXCEPTION_TEXT);
				}
			}

			private static void Fetch_Method3()
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

				if (largest == 0.0F)
					throw new FrontendException(NOT_FOUND_EXCEPTION_TEXT);

				string versionStr = largest.ToString("F1");

				string[] files = Directory.GetFiles(registry.GetValue(versionStr).ToString(), "MSBuild.exe", SearchOption.AllDirectories);
				if (files.Length == 0)
					throw new FrontendException(NOT_FOUND_EXCEPTION_TEXT);
				Path = files[0];

				switch (largest)
				{
					case 14:
						ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_0;
						break;

					case 15:
						ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_1;
						break;

					default:
						throw new FrontendException(NOT_FOUND_EXCEPTION_TEXT);
				}
			}
		}

		public MSBuildProcess()
		{
			FilePath = Info.Path;
		}

		public override void Build(string ProjectPath, ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration, ProjectBase.ProfileBase.PlatformArchitectures PlatformType)
		{
			Start(string.Format("\"{0}\" /t:build /p:configuration={1} /p:platform={2}", ProjectPath, BuildConfiguration.ToString().ToLower(), PlatformType.ToString()));
		}

		public override void Build(ProjectBase.ProfileBase ProjectProfile)
		{
			string projFilePath = ProjectProfile.IntermediateDirectory + ProjectProfile.AssemblyName;

			MSBuildProjectGenerator projectGenerator = null;

			if (ProjectProfile is CPPProject.Profile)
			{
				MicrosoftVCProjectGenerator generator = new MicrosoftVCProjectGenerator();
				projectGenerator = generator;
				generator.ToolsVersion = Info.ToolsVersion;

				CPPProject project = (CPPProject)ProjectProfile.Project;

				if (!string.IsNullOrEmpty(project.ResourceDefinition.IconPath))
				{
					string icoFilePath = $"{projFilePath}Icon{EnvironmentHelper.IconExtension}";
					IconMaker.Make(project.ResourceDefinition.IconPath, icoFilePath);

					project.ResourceDefinition.IconPath = icoFilePath.Replace("\\", "\\\\");
				}

				string resourceContent = generator.GenerateResourceDefinition(project);
				if (!string.IsNullOrEmpty(resourceContent))
				{
					string resourcePath = $"{projFilePath}Resources.rc";
					File.WriteAllText(resourcePath, resourceContent);

					project.AddResource(resourcePath);
				}

				projFilePath += ".vcxproj";
			}
			else if (ProjectProfile is CSProject.Profile)
			{
				projectGenerator = new MicrosoftCSProjectGenerator();

				projFilePath += ".csproj";
			}

			File.WriteAllText(projFilePath, projectGenerator.Generate(ProjectProfile.Project, false));

			Build(projFilePath, ProjectProfile.BuildConfiguration, ProjectProfile.PlatformArchitecture);
		}

		public override void Rebuild(string ProjectPath, ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration, ProjectBase.ProfileBase.PlatformArchitectures Architecture)
		{
			Start(string.Format("\"{0}\" /t:clean;build /p:configuration={1} /p:platform={2}", ProjectPath, BuildConfiguration.ToString().ToLower(), Architecture.ToString()));
		}

		public override void Clean(string ProjectPath)
		{
			Start(string.Format("\"{0}\" /t:clean", ProjectPath));
		}
	}
}