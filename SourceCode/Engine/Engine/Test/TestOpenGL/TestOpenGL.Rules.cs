// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class TestOpenGLRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "TestOpenGL"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "TestOpenGL"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.Executable; }
            }

            public override string[] DependencyModulesName
            {
                get { return new string[] { "Parallelizing", "MemoryManagement", "Containers", "GLEW", "GLFW", "GLM" }; }
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