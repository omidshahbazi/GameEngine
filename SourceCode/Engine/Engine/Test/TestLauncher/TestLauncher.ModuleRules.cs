// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class TestLauncherModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "TestLauncher"; }
        }

        public class BuildRules : BuildRulesBase
        {
            public override string TargetName
            {
                get { return "TestLauncher"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.Executable; }
            }

            public override string[] PrivateDependencyModulesName
            {
                get { return new string[] { "CoreSystem", "GameObjectSystem", "MemoryManagement", "ResourceSystem", "WindowUtility", "FileUtility", "FontSystem", "InputSystem", "Mathematics" }; }
            }

            public override bool GenerateRenderDocSettings
            {
                get { return true; }
            }
        }
    }
}