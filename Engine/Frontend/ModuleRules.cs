// Copyright 2016-2020 ?????????????. All Rights Reserved.

using Engine.Frontend.Project;

namespace Engine
{
	public abstract class ModuleRules
	{
		public enum Configurations
		{
			Debug = ProjectBase.ProfileBase.BuildConfigurations.Debug,
			Release = ProjectBase.ProfileBase.BuildConfigurations.Release
		}

		public enum Platforms
		{
			x86 = ProjectBase.ProfileBase.PlatformArchitectures.x86,
			x64 = ProjectBase.ProfileBase.PlatformArchitectures.x64
		}

		public enum LibraryUseTypes
		{
			Executable = 0,
			DynamicLibrary,
			StaticLibrary,
			ProjectFile,
			UseOnly
		}

		public enum Priorities
		{
			PreBuildProcess = 0,
			InBuildProcess,
			PostBuildProcess
		}

		public const string FilePostfix = ".ModuleRules.cs";

		public abstract class BuildRulesBase
		{
			public abstract string TargetName
			{
				get;
			}

			public virtual Configurations Configuration
			{
				get { return Configurations.Debug | Configurations.Release; }
			}

			public virtual Platforms Platform
			{
				get { return Platforms.x86 | Platforms.x64; }
			}

			public abstract LibraryUseTypes LibraryUseType
			{
				get;
			}

			public virtual string[] PreprocessorDefinitions
			{
				get { return null; }
			}

			public virtual string[] PrivateDependencyModuleNames
			{
				get { return null; }
			}

			public virtual string[] PublicDependencyModuleNames
			{
				get { return null; }
			}

			public virtual string[] DependencyStaticLibraries
			{
				get { return null; }
			}

			public virtual Priorities Priority
			{
				get { return Priorities.InBuildProcess; }
			}

			public virtual string[] IncludePaths
			{
				get { return null; }
			}

			public virtual string[] BinaryPaths
			{
				get { return null; }
			}

			public virtual string[] LibraryPaths
			{
				get { return null; }
			}

			public virtual bool GenerateReflection
			{
				get { return false; }
			}

			public virtual bool GenerateRenderDocSettings
			{
				get { return false; }
			}
		}

		public abstract string Name
		{
			get;
		}

		public BuildRulesBase BuildRules
		{
			get;
			set;
		}
	}
}