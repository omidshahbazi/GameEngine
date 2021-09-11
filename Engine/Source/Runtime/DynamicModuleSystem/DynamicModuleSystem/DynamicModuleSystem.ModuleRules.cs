// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.DynamicModuleSystem
{
	class DynamicModuleSystemModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "DynamicModuleSystem"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "DynamicModuleSystem"; }
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