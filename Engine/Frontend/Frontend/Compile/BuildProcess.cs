// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Project;

namespace Engine.Frontend.Compile
{
	abstract class BuildProcess : CommandLineProcess
	{
		public abstract void Build(string ProjectPath, ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration, ProjectBase.ProfileBase.PlatformArchitectures PlatformType);

		public abstract void Build(ProjectBase.ProfileBase ProjectProfile);

		public abstract void Rebuild(string ProjectPath, ProjectBase.ProfileBase.BuildConfigurations BuildConfiguration, ProjectBase.ProfileBase.PlatformArchitectures PlatformType);

		public abstract void Clean(string ProjectPath);
	}
}