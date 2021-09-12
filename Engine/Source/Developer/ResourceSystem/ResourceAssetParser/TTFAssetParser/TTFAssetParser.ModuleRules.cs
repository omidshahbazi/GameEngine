// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Developer.ResourceSystem.ResourceAssetParser
{
	class TTFAssetParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "TTFAssetParser"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "TTFAssetParser"; }
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