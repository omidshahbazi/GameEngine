// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class STBRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "STB"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "STB"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.StaticLibrary; }
			}
		}
    }
}