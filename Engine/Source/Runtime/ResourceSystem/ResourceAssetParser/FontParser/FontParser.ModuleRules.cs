// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem.ResourceAssetParser
{
	class FontParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "FontParser"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "FontParser"; }
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
				get { return new string[] { "MeshParser", "TextureParser" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Containers", "MathContainers", "RenderDevice", "RenderCommon", "MemoryManagement" }; }
			}
		}
	}
}