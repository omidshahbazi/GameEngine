// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class ResourceSystemRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "ResourceSystem"; }
        }

        public class Rule : RuleBase
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