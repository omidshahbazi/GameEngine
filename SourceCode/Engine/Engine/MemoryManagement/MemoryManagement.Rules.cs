// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class MemoryManagementRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "MemoryManagement"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "MemoryManagement"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModulesName
			{
				get { return new string[] { "Platform", "Debugging" }; }
			}

			public override string[] IncludeModulesName
			{
				get { return new string[] { "Containers" }; }
			}

			public override string[] PreprocessorDefinitions
			{
				get { return new string[] { "LAEK_DETECTION", "_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING" }; }
			}
		}
	}
}