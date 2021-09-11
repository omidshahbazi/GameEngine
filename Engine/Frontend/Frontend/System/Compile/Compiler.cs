// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;
using Engine.Frontend.Utilities;

namespace Engine.Frontend.System.Compile
{
	delegate void MessageRaisedEventHandler(string Text);

	class Compiler
	{
		private const string BuildFailed = "Build FAILED.";
		private const string ErrorPattern = " error ";
		private const string WarningPattern = " warning ";
		private const string LinkErrorPattern = " LNK";

		private static BuildProcess process = null;

		public event MessageRaisedEventHandler ErrorRaised;
		public event MessageRaisedEventHandler WarningRaised;

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

				//Console.WriteLine(line);

				if (line.Contains(ErrorPattern))
				{
					//	try
					//	{
					//		if (line.Contains(LinkErrorPattern))
					//		{
					//			line = line.Split('[')[0].Substring(line.IndexOf(':') + 2);
					//			OnErrorRaised(line);
					//		}
					//		else
					//		{
					//			string[] parts = line.Split('[')[0].Split(':');
					//			string[] fileData = parts[1].Split('(');
					//			OnErrorRaised(parts[2].TrimStart() + " in " + Path.GetFileName(fileData[0]) + (fileData.Length > 1 ? " Line (" + fileData[1].Split(')')[0] + ")" : "") + parts[3] + parts[4] + parts[5]);
					//		}
					//	}
					//	catch (Exception e)
					//	{
					if (ErrorRaised != null)
						ErrorRaised(line);
					//}

					wasSuccessful = false;
				}
				else if (line.Contains(WarningPattern))
				{
					if (WarningRaised != null)
						WarningRaised(line);
				}
			}

			if (!wasSuccessful)
				throw new FrontendException("Compile failed");
		}
	}
}