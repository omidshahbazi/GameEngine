// Copyright 2016-2017 ?????????????. All Rights Reserved.

namespace Engine.Frontend
{
	public abstract class BuildRules
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

		public const string BuildRuleFilePostfix = ".Rules.cs";
		public const string NamespacePrefix = "Engine.Frontend.";

		public abstract class RuleBase
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

			public virtual string ProjectFilePath
			{
				get { return string.Empty; }
			}

			public virtual string[] PreprocessorDefinitions
			{
				get { return null; }
			}

			public virtual string[] DependencyModulesName
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

			public virtual string[] AdditionalIncludeDirectory
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
		}

		public abstract string ModuleName
		{
			get;
		}

		public string Path
		{
			get;
			set;
		}

		public RuleBase[] Rules
		{
			get;
			set;
		}
	}
}