// Copyright 2016-2017 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;

namespace Engine.Frontend.System.Compile
{
	abstract class BuildProcess : CommandLineProcess
	{
		public BuildProcess(string FilePath) :
			base(FilePath)
		{
		}

		public abstract void Build(ProjectBase.ProfileBase ProjectProfile);
	}
}