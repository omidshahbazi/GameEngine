// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class GameObjectSystemRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "GameObjectSystem"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "GameObjectSystem"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] DependencyModulesName
			{
				get { return new string[] { "Containers", "MathContainers", "MemoryManagement", "Rendering", "FontSystem", "ResourceSystem", "Profiler" }; }
			}
		}
    }
}