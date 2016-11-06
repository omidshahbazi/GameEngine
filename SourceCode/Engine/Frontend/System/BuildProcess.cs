// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System.Diagnostics;

namespace Engine.Frontend
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