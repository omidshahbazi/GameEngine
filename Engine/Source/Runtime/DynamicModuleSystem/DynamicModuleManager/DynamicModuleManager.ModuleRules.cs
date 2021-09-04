// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class DynamicModuleManagerModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "DynamicModuleManager"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "DynamicModuleManager"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "FileUtility" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "DynamicModuleDefinition", "Containers", "MemoryManagement", "Threading", "Platform" }; }
			}
		}
	}
}