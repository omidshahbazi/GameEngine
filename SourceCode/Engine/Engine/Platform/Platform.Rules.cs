// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class PlatformRules : BuildRules
    {
        public override string ModuleName
        {
            get { return "Platform"; }
        }

        public class Rule : RuleBase
        {
            public override string TargetName
            {
                get { return "Platform"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }

            public override string[] DependencyModulesName
            {
                get { return new string[] { "Common", "Debugging", "GLEW" }; }
            }
			
			public override string[] DependencyStaticLibraries
			{
				get { return new string[] { "rpcrt4.lib", "opengl32.lib", "Shcore.lib" }; }
            }

            public override string[] PreprocessorDefinitions
            {
                get { return new string[] { "_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING" }; }
            }
        }
    }
}