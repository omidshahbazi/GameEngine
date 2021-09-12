// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Developer.ThirdParty
{
    class STBModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "STB"; }
        }

        public class BuildRules : BuildRulesBase
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