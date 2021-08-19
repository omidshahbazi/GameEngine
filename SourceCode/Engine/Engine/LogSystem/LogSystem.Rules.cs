// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class LogSystemRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "LogSystem"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "LogSystem"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }

            public override string[] DependencyModulesName
            {
                get { return new string[] { "Common", "Platform", "Threading", "FileUtility", "Debugging" }; }
            }
        }
	}
}