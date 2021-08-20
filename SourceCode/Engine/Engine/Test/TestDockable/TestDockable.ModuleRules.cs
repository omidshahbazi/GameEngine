// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class TestDockableModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "TestDockable"; }
        }

        public class BuildRules : BuildRulesBase
        {
            public override string TargetName
            {
                get { return "TestDockable"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.Executable; }
            }

            public override string[] PrivateDependencyModuleNames
            {
                get { return new string[] { "FileUtility", "Rendering", "EditorGUI", "ResourceSystem", "FontSystem", "LogSystem" }; }
            }

            public override bool GenerateRenderDocSettings
			{
				get { return true; }
			}
        }
    }
}