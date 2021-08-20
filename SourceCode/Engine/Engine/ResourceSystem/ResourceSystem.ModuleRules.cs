// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class ResourceSystemModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "ResourceSystem"; }
        }

        public class BuildRules : BuildRulesBase
        {
            public override string TargetName
            {
                get { return "ResourceSystem"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }

            public override string[] IncludeModulesName
            {
                get { return new string[] { "ResourceSystem" }; }
            }

            public override string[] PrivateDependencyModulesName
            {
                get { return new string[] { "MathContainers", "ResourceAssetParser", "FontSystem", "DataUtility", "YAML" }; }
            }

            public override string[] PublicDependencyModulesName
            {
                get { return new string[] { "Containers", "Threading", "MemoryManagement", "FileUtility", "Rendering" }; }
            }

            public override bool GenerateReflection
            {
                get { return true; }
            }
        }
    }
}