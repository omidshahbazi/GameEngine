// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System.Diagnostics;
using System.IO;

namespace Engine.Frontend.System.Compile
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

		public virtual string FilePath
		{
			get;
			set;
		}

		public CommandLineProcess()
		{
			process = new Process();
			ProcessStartInfo info = process.StartInfo;
			info.CreateNoWindow = true;
			info.RedirectStandardOutput = true;
			info.UseShellExecute = false;
		}

		public void Start(string Arguments = "")
		{
			process.StartInfo.FileName = FilePath;

			process.StartInfo.Arguments = Arguments;

			process.Start();
		}
	}
}