// Copyright 2012-2015 ?????????????. All Rights Reserved.
using System.Diagnostics;

namespace Frontend
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

		public override void Build (string FilePath)
		{
			base.Start("\"" + FilePath + "\" /t:build /p:configuration=" + BuildSystem.BuildConfiguration.ToString().ToLower());
		}
	}
}