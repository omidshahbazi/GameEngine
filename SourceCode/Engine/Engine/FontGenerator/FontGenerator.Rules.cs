// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class FontGeneratorRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "FontGenerator"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "FontGenerator"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.Executable; }
            }

            public override string[] DependencyModulesName
            {
                get { return new string[] { "FontSystem", "Utility", "Debugging" }; }
            }
        }
    }
}