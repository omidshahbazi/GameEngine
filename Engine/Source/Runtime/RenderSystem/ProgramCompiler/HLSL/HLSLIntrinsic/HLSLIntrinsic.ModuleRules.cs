// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class HLSLIntrinsicModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "HLSLIntrinsic"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "HLSLIntrinsic"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "APIIntrinsic", "DynamicModuleDefinition", "ASTToHLSLCompiler" }; }
			}
		}
	}
}