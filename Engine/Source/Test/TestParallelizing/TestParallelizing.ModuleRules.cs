// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Test
{
    class TestParallelizingModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "TestParallelizing"; }
        }

        public class BuildRules : BuildRulesBase
        {
            public override string TargetName
            {
                get { return "TestParallelizing"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.Executable; }
            }

            public override string[] PrivateDependencyModuleNames
            {
                get { return new string[] { "Common", "Debugging", "Platform", "MemoryManagement", "Parallelizing", "Threading" }; }
            }
        }
    }
}