// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.MemoryManagement
{
	class AllocatorsModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Allocators"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "Allocators"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Platform" }; }
			}

			public override string[] PreprocessorDefinitions
			{
				get { return new string[] { "LEAK_DETECTION", "CORRUPTED_HEAP_DETECTION" }; }
			}
		}
	}
}