// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class NetworkingRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "Networking"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "Networking"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }

            public override string[] DependencyModulesName
            {
                get { return new string[] { "MemoryManagement", "Platform", "Debugging" }; }
            }
        }
	}
}