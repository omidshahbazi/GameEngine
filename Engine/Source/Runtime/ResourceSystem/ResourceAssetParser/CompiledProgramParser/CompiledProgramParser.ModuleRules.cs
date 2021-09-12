// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ResourceSystem.ResourceAssetParser
{
	class CompiledProgramParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "CompiledProgramParser"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "CompiledProgramParser"; }
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