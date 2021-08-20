// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
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

			public override string[] PrivateDependencyModulesName
			{
				get { return new string[] { "Common", "Debugging", "Platform", "LogSystem" }; }
			}

			public override string[] PublicDependencyModulesName
			{
				get { return new string[] { "Containers", "MemoryManagement", "Threading" }; }
			}
		}
	}
}