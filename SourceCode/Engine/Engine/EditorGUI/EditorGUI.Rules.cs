// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class EditorGUIRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "EditorGUI"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "EditorGUI"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }

            public override string[] DependencyModulesName
            {
                get { return new string[] { "WindowUtility", "Rendering", "ResourceSystem", "FontSystem", "Debugging" }; }
            }
        }
    }
}