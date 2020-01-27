// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class TestFileRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "TestFile"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "TestFile"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.Executable; }
            }

            public override string[] DependencyModulesName
            {
                get { return new string[] { "Common", "Debugging", "Platform", "MemoryManagement", "Parallelizing", "Threading" }; }
            }
        }
    }
}