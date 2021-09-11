// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem
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

            public override string[] PrivateDependencyModuleNames
            {
                get { return new string[] { "JSON" }; }
            }

            public override string[] PublicDependencyModuleNames
            {
                get { return new string[] { "Containers", "Common", "MemoryManagement" }; }
            }

            public override bool GenerateReflection
            {
                get { return true; }
            }
        }
    }
}