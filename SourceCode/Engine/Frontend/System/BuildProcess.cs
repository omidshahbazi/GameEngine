// Copyright 2012-2015 ?????????????. All Rights Reserved.
using System.Diagnostics;

namespace Frontend
{
	abstract class BuildProcess : CommandLineProcess
	{
		public BuildProcess(string FilePath) :
		base(FilePath)
		{
		}

		public abstract void Build (string FilePath);
	}
}