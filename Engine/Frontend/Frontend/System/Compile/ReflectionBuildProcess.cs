// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Utilities;
using System;
using System.IO;

namespace Engine.Frontend.System.Compile
{
	class ReflectionBuildProcess : CommandLineProcess
	{
		public ReflectionBuildProcess()
		{
			if (!File.Exists(EnvironmentHelper.ReflectionToolPath))
				throw new FrontendException($"Couldn't find {EnvironmentHelper.ReflectionToolName} at {EnvironmentHelper.ReflectionToolPath}");

			FilePath = EnvironmentHelper.ReflectionToolPath;
		}

		public bool Build(string FilePath, string OutputBaseFileName)
		{
			Start("\"" + FilePath + "\" \"" + OutputBaseFileName + "\"");

			string error = "";

			//while (!Output.EndOfStream)
			//	error += Output.ReadLine() + Environment.NewLine;
			Wait();

			if (ExitCode == 0)
				return true;
			else if (ExitCode == 1)
				return false;

			throw new FrontendException($"Parsing {FilePath}  for reflection has error(s): {error}");
		}
	}
}