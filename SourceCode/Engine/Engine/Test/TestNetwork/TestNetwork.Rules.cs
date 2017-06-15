// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class TestNetworkRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "TestNetwork"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "TestNetwork"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.Executable; }
            }

            public override string[] DependencyModulesName
            {
                get { return new string[] { "Common", "Networking", "Platform", "MemoryManagement", "Parallelizing", "Threading", "Debugging" }; }
            }
        }
    }
}