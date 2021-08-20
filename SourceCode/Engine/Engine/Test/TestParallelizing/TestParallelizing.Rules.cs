// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class TestParallelizingRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "TestParallelizing"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "TestParallelizing"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.Executable; }
            }

            public override string[] PrivateDependencyModulesName
            {
                get { return new string[] { "Common", "Debugging", "Platform", "MemoryManagement", "Parallelizing", "Threading" }; }
            }
        }
    }
}