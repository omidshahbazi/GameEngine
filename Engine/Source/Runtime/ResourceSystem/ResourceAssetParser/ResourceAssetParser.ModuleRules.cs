// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class ResourceAssetParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ResourceAssetParser"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "ResourceAssetParser"; }
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
				get { return new string[] { "STB", "FreeType" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Containers", "MathContainers", "RenderDevice", "RenderCommon", "ProgramCompilerCommon", "STB", "FreeType" }; }
			}

			public override string[] DependencyStaticLibraries
			{
				get { return new string[] { "opengl32.lib", "Glu32.lib" }; }
			}
		}
	}
}