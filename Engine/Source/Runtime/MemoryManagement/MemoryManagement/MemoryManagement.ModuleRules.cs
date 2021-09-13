// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class MemoryManagementModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "MemoryManagement"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "MemoryManagement"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Common", "Allocators" }; }
			}
		}
	}
}