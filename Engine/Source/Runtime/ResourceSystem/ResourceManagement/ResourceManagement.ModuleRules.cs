// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem
{
    class ResourceManagementModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "ResourceManagement"; }
        }

        public class BuildRules : BuildRulesBase
        {
            public override string TargetName
            {
                get { return "ResourceManagement"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }

            public override string[] PrivateDependencyModuleNames
            {
                get { return new string[] { "Common", "OBJAssetParser", "RenderSystem", "FileUtility", "Containers" }; }
			}

            public override string[] PublicDependencyModuleNames
            {
                get { return new string[] { "MemoryManagement", "ResourceSystem", "ResourceCommon" }; }
            }
        }
    }
}