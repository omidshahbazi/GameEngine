// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class CoreSystemModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "CoreSystem"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "CoreSystem"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "FileUtility", "InputSystem", "FontSystem", "GameObjectSystem", "ResourceManagement", "Profiler", "DynamicModuleSystem" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Common", "MemoryManagement", "WindowUtility", "TimeUtility", "Containers", "RenderSystem" }; }
			}
		}
	}
}