// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Editor
{
    class EditorGUIModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "EditorGUI"; }
        }

        public class BuildRules : BuildRulesBase
        {
            public override string TargetName
            {
                get { return "EditorGUI"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }

            public override string[] PrivateDependencyModuleNames
            {
                get { return new string[] { "WindowUtility", "RenderSystem", "RenderCommon", "ResourceSystem", "ResourceManagement", "FontSystem", "Debugging" }; }
            }
        }
    }
}