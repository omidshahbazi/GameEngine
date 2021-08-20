// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
    class PlatformModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "Platform"; }
        }

        public class BuildRules : BuildRulesBase
        {
            public override string TargetName
            {
                get { return "Platform"; }
            }

            public override LibraryUseTypes LibraryUseType
            {
                get { return LibraryUseTypes.DynamicLibrary; }
            }

            public override string[] PrivateDependencyModuleNames
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