// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class ResourceCommonModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "ResourceCommon"; }
        }

        public class BuildRules : BuildRulesBase
        {
            public override string TargetName
            {
                get { return "ResourceCommon"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }

            public override string[] PublicDependencyModuleNames
            {
                get { return new string[] { "Common", "Containers" }; }
            }
        }
    }
}