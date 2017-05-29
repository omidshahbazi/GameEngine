// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Project.Generator;
using System.IO;
using System;

namespace Engine.Frontend.System.Compile
{
	class MSBuildProcess : BuildProcess
	{
		private static string mSBuildPath = "";
		private static string MSBuildPath
		{
			get
			{
				if (string.IsNullOrEmpty(mSBuildPath))
				{
					//mSBuildPath = "%ProgramFiles(x86)%/MSBuild/12.0/Bin/MSBuild.exe";
					mSBuildPath = "C:/Program Files (x86)/MSBuild/14.0/Bin/MSBuild.exe";
				}

				return mSBuildPath;
			}
		}

		public MSBuildProcess() :
			base(MSBuildPath)
		{
		}

		public override void Build(string ProjectPath, ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration, ProjectBase.ProfileBase.PlatformTypes PlatformType)
		{
			Start(string.Format("\"{0}\" /t:{1} /p:configuration={2} /p:Platform={3}", ProjectPath, "build", BuildConfiguration.ToString().ToLower(), PlatformType.ToString()));
		}

		public override void Build(ProjectBase.ProfileBase ProjectProfile)
		{
			string projPath = ProjectProfile.IntermediatePath + ProjectProfile.AssemblyName + ".csproj";

			ProjectGeneratorBase projectGenerator = null;

			if (ProjectProfile is CPPProject.Profile)
			{
				MicrosoftVCProjectGenerator generator = new MicrosoftVCProjectGenerator();
				projectGenerator = generator;
				generator.ToolsVersion = MicrosoftVCProjectGenerator.ToolsVersions.v14_0;
			}
			else if (ProjectProfile is CSProject.Profile)
				projectGenerator = new MicrosoftCSProjectGenerator();

			File.WriteAllText(projPath, projectGenerator.Generate(ProjectProfile.Project));

			Build(projPath, ProjectProfile.BuildConfiguration, ProjectProfile.PlatformType);
		}
	}
}