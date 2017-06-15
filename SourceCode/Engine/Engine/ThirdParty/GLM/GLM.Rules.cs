// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class GLMRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "GLM"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "GLM"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }

            public override string[] DependencyModulesName
            {
                get { return new string[] { "MemoryManagement", "Platform", "Debugging" }; }
            }
        }
	}
}