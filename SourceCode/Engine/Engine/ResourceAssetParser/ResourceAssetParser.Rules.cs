// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class ResourceAssetParserRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "ResourceAssetParser"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "ResourceAssetParser"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }

            public override string[] DependencyModulesName
            {
                get { return new string[] { "Containers", "MathContainers", "Rendering", "STB" }; }
            }
        }
    }
}