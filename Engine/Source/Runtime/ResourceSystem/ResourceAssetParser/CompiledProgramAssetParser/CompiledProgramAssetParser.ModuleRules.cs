// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem.ResourceAssetParser
{
	class CompiledProgramAssetParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "CompiledProgramAssetParser"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "CompiledProgramAssetParser"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Containers",  "RenderDevice", "RenderCommon" }; }
			}
		}
	}
}