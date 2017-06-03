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
		private MicrosoftVCProjectGenerator.ToolsVersions toolsVersion;
		private string mSBuildPath = "";

		public override string FilePath
		{
			get
			{
				if (string.IsNullOrEmpty(mSBuildPath))
				{
					const string path = @"SOFTWARE\Microsoft\MSBuild\ToolsVersions\";

					RegistryKey registry = Registry.LocalMachine.OpenSubKey(path);

					string[] versions = registry.GetSubKeyNames();

					int index = Array.IndexOf(versions, toolsVersion.ToString().Substring(1).Replace('_', '.'));
					if (index == -1)
						throw new Exception("There isn't any Microsoft Build Tool installed on the machine");

					string version = versions[index];

					registry = Registry.LocalMachine.OpenSubKey(path + version + @"\");

					mSBuildPath = registry.GetValue("MSBuildToolsRoot") + version + "/Bin/MSBuild.exe";
				}

				return mSBuildPath;
			}
			set { }
		}

		public MSBuildProcess()
		{
			toolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_0;
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
				generator.ToolsVersion = toolsVersion;

				projPath += ".vcxproj";
			}
			else if (ProjectProfile is CSProject.Profile)
			{
				projectGenerator = new MicrosoftCSProjectGenerator();

				projPath += ".csproj";
			}

			File.WriteAllText(projPath, projectGenerator.Generate(ProjectProfile.Project));

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