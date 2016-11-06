// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System.Diagnostics;
using System.IO;

namespace Engine.Frontend
{
	class CommandLineProcess
	{
		private Process process = null;

		public StreamReader Output
		{
			get { return process.StandardOutput; }
		}

		public int ExitCode
		{
			get { return process.ExitCode; }
		}

		public CommandLineProcess(string FilePath)
		{
			process = new Process();
			ProcessStartInfo info = process.StartInfo;
			info.CreateNoWindow = true;
			info.RedirectStandardOutput = true;
			info.UseShellExecute = false;
			info.FileName = FilePath;
		}

		public void Start(string Arguments = "")
		{
			process.StartInfo.Arguments = Arguments;

			process.Start();
		}
	}
}