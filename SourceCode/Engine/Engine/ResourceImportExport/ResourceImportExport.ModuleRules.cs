// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class ResourceImportExportModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "ResourceImportExport"; }
        }

        public class BuildRules : BuildRulesBase
        {
            public override string TargetName
            {
                get { return "ResourceImportExport"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }

            public override string[] PrivateDependencyModulesName
            {
                get { return new string[] { "YAML" }; }
            }

            public override string[] PublicDependencyModulesName
            {
                get { return new string[] { "Containers", "Common" }; }
            }

            public override bool GenerateReflection
            {
                get { return true; }
            }
        }
    }
}