// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Developer.ResourceSystem.ResourceAssetParser
{
	class TTFParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "TTFParser"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "TTFParser"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PreprocessorDefinitions
			{
				get { return new string[] { "FONTSYSTEM_API=" }; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "FreeType" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Containers", "MathContainers", "RenderDevice", "MemoryManagement" }; }
			}

			public override string[] DependencyStaticLibraries
			{
				get { return new string[] { "opengl32.lib", "Glu32.lib" }; }
			}
		}
	}
}