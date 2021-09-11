// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class ParallelizingModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Parallelizing"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "Parallelizing"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Common", "Debugging", "Platform" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Containers", "MemoryManagement", "Threading" }; }
			}
		}
	}
}