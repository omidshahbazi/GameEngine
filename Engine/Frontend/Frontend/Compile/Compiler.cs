// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Utilities;

namespace Engine.Frontend.Compile
{
	delegate void OutputMessageEventHandler(string Text);

	class Compiler
	{
		private const string BuildFailed = "Build FAILED.";
		private const string ErrorPattern = " error ";
		private const string WarningPattern = " warning ";
		private const string LinkErrorPattern = " LNK";

		private static BuildProcess process = null;

		public event OutputMessageEventHandler OnErrorMessage;
		public event OutputMessageEventHandler OnWarningMessage;

		public Compiler()
		{
			if (EnvironmentHelper.ManagedRuntime == EnvironmentHelper.ManagedRuntimes.DotNet)
				process = new MSBuildProcess();
			else
				process = new MonoBuildProcess();
		}

		public void Build(string ProjectPath, ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration, ProjectBase.ProfileBase.PlatformArchitectures PlatformType)
		{
			process.Build(ProjectPath, BuildConfiguration, PlatformType);

			CheckResult();
		}

		public void Build(ProjectBase.ProfileBase ProjectProfile)
		{
			process.Build(ProjectProfile);

			CheckResult();
		}

		private void CheckResult()
		{
			bool wasSuccessful = true;

			while (!process.Output.EndOfStream)
			{
				string line = process.Output.ReadLine();

				if (line.Contains(ErrorPattern))
				{
					if (OnErrorMessage != null)
						OnErrorMessage(line);

					wasSuccessful = false;
				}
				else if (line.Contains(WarningPattern))
				{
					if (OnWarningMessage != null)
						OnWarningMessage(line);
				}
			}

			if (!wasSuccessful)
				throw new FrontendException("Compile failed");
		}
	}
}
