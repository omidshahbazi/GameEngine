// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;

namespace Engine.Frontend.System.Compile
{
	abstract class BuildProcess : CommandLineProcess
	{
		public abstract void Build(string ProjectPath, ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration, ProjectBase.ProfileBase.PlatformTypes PlatformType);

		public abstract void Build(ProjectBase.ProfileBase ProjectProfile);

		public abstract void Rebuild(string ProjectPath, ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration, ProjectBase.ProfileBase.PlatformTypes PlatformType);

		public abstract void Clean(string ProjectPath);
	}
}