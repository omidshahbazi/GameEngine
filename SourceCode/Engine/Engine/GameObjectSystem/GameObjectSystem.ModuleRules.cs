// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class GameObjectSystemModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "GameObjectSystem"; }
        }

        public class BuildRules : BuildRulesBase
        {
            public override string TargetName
            {
                get { return "GameObjectSystem"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModulesName
			{
				get { return new string[] { "Containers", "MathContainers", "MemoryManagement", "Rendering", "FontSystem", "ResourceSystem", "Profiler" }; }
			}
		}
    }
}