// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.DynamicModuleSystem
{
    class DynamicModuleDefinitionModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "DynamicModuleDefinition"; }
        }

        public class BuildRules : BuildRulesBase
        {
            public override string TargetName
            {
                get { return "DynamicModuleDefinition"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Containers", "MemoryManagement" }; }
			}
		}
    }
}