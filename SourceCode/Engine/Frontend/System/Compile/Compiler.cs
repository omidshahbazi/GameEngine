// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;

namespace Engine.Frontend.System.Compile
{
	delegate void ErrorRaisedEventHandler(string Text);

	class Compiler
	{
		private const string BuildFailed = "Build FAILED.";
		private const string ErrorPattern = " error ";
		private const string LinkErrorPattern = " LNK";

		private static BuildProcess process = null;

		public event ErrorRaisedEventHandler ErrorRaised;

		public Compiler()
		{
			if (EnvironmentHelper.ManagedRuntime == EnvironmentHelper.ManagedRuntimes.DotNet)
				process = new MSBuildProcess();
			else
				process = new MonoBuildProcess();
		}

		public bool Build(ProjectBase.ProfileBase ProjectProfile)
		{
			process.Build(ProjectProfile);

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
					OnErrorRaised(line);
					//}

					wasSuccessful = false;
				}
			}

			return wasSuccessful;
		}

		private void OnErrorRaised(string Text)
		{
			if (ErrorRaised != null)
				ErrorRaised(Text);
		}
	}
}
