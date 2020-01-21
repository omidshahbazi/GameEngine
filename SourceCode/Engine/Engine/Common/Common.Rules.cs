// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class CommonRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "Common"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "Common"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }
        }
    }
}