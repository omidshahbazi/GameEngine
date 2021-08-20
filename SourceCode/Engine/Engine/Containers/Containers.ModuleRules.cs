// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class ContainersModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Containers"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "Containers"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Common", "MemoryManagement", "Debugging", "Platform", "Mathematics" }; }
			}
		}
	}
}