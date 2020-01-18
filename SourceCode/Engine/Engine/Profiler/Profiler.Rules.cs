// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class ProfilerRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "Profiler"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "Profiler"; }
			}

			public override string[] DependencyModulesName
			{
				get { return new string[] { "Common", "Containers", "MemoryManagement", "Platform", "Utility" }; }
			}

			public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }
        }
	}
}