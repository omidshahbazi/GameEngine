// Copyright 2016-2017 ?????????????. All Rights Reserved.

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

		public override void Build(string FilePath)
		{
			base.Start(string.Format("\"{0}\" /t:{1} /p:configuration={2} /p:Platform={3}", FilePath, "build", BuildSystem.BuildConfiguration.ToString().ToLower(), BuildSystem.PlatformType.ToString()));
		}
	}
}