// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class TestReflectionRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "TestReflection"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "TestReflection"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.Executable; }
			}

			public override bool GenerateReflection
			{
				get { return true; }
			}
		}
    }
}