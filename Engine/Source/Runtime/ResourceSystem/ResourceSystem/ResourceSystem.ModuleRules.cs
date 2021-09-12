// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem
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

            public override string[] PrivateDependencyModuleNames
            {
                get { return new string[] { "MemoryManagement", "Platform", "RenderSystem", "FontSystem", "ResourceCommon", "ResourceImportExport", "ProgramCompiler", "ImageAssetParser", "TextureAssetParser", "MeshAssetParser", "TTFAssetParser", "FontAssetParser", "OBJAssetParser", "TextAssetParser", "ProgramAssetParser", "CompiledProgramAssetParser", "TTFAssetParser", "FontAssetParser", "Debugging", "JSON", "DataUtility" }; }
			}

            public override string[] PublicDependencyModuleNames
            {
                get { return new string[] { "Common", "Containers", "FileUtility", "Threading" }; }
            }
        }
    }
}