// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class TestDockableRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "TestDockable"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "TestDockable"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.Executable; }
            }

            public override string[] DependencyModulesName
            {
                get { return new string[] { "Utility", "Rendering", "EditorGUI", "ResourceSystem" }; }
            }

            public override bool GenerateRenderDocSettings
			{
				get { return true; }
			}
        }
    }
}