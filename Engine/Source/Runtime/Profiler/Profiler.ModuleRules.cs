// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class ProfilerModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "Profiler"; }
        }

        public class BuildRules : BuildRulesBase
        {
            public override string TargetName
            {
                get { return "Profiler"; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Common", "Containers", "MemoryManagement", "Platform", "FileUtility", "TimeUtility" }; }
			}

			public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }
        }
	}
}