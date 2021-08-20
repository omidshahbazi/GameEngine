// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class LogSystemModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "LogSystem"; }
        }

        public class BuildRules : BuildRulesBase
        {
            public override string TargetName
            {
                get { return "LogSystem"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }

            public override string[] PrivateDependencyModuleNames
            {
                get { return new string[] { "Common", "Containers", "Platform", "Threading", "FileUtility", "Debugging" }; }
            }
        }
	}
}