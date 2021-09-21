// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class EntityComponentSystemModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "EntityComponentSystem"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "EntityComponentSystem"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Common", "Containers", "Allocators", "Platform", "Debugging" }; }
			}
		}
	}
}