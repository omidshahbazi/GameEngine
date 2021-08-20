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

            public override string[] PrivateDependencyModulesName
            {
                get { return new string[] { "Common", "Containers", "Platform", "Threading", "FileUtility", "Debugging" }; }
            }
        }
	}
}