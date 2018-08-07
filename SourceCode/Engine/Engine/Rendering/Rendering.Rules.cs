// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class RenderingRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "Rendering"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "Rendering"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] DependencyModulesName
			{
				get { return new string[] { "Containers", "MemoryManagement", "GLEW", "GLFW", "GLM" }; }
			}

			public override string[] DependencyStaticLibraries
            {
                get { return new string[] { "opengl32.lib" }; }
            }

            public override string[] PreprocessorDefinitions
            {
                get { return new string[] { "GLM_ENABLE_EXPERIMENTAL" }; }
            }
        }
    }
}