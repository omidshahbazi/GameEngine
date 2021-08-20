// Copyright 2016-2020 ?????????????. All Rights Reserved.

namespace Engine.Frontend
{
	public abstract class ModuleRules
	{
		public enum Configurations
		{
			Debug = 0,
			Release
		}

		public enum Platforms
		{
			x86 = 0,
			x64
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

		public const string ModuleRuleFilePostfix = ".ModuleRules.cs";
		public const string NamespacePrefix = "Engine.Frontend.";

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

			public virtual string[] PrivateDependencyModulesName
			{
				get { return null; }
			}

			public virtual string[] PublicDependencyModulesName
			{
				get { return null; }
			}

			public virtual string[] DependencyStaticLibraries
			{
				get { return null; }
			}

			public virtual string[] IncludeModulesName
			{
				get { return null; }
			}

			public virtual Priorities Priority
			{
				get { return Priorities.InBuildProcess; }
			}

			public virtual string[] IncludesPath
			{
				get { return null; }
			}

			public virtual string[] BinariesPath
			{
				get { return null; }
			}

			public virtual string[] LibrariesPath
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

		public string Path
		{
			get;
			set;
		}

		public BuildRulesBase[] BuildRules
		{
			get;
			set;
		}
	}
}