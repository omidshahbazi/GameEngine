// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
    class PlatformModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "Platform"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public PlatformModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("GLEW");

			DependencyStaticLibraries.Add("rpcrt4.lib");
			DependencyStaticLibraries.Add("opengl32.lib");
			DependencyStaticLibraries.Add("Shcore.lib");

			PreprocessorDefinitions.Add("_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING");
		}
    }
}