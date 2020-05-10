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

            public override string[] IncludeModulesName
            {
                get { return new string[] { "ResourceSystem", "FontSystem" }; }
            }

            public override string[] DependencyModulesName
            {
                get { return new string[] { "Containers", "MathContainers", "Rendering", "STB", "FreeType" }; }
            }

            public override string[] DependencyStaticLibraries
            {
                get { return new string[] { "opengl32.lib", "Glu32.lib" }; }
            }
        }
    }
}