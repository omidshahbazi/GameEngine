// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class TestLauncherRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "TestLauncher"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "TestLauncher"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.Executable; }
            }

            public override string[] DependencyModulesName
            {
                get { return new string[] { "CoreSystem", "GameObjectSystem", "MemoryManagement", "ResourceSystem", "Utility" }; }
            }
        }
    }
}