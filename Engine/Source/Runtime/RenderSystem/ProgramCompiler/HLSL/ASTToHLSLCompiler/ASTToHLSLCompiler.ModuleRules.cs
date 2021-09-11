// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler.HLSL
{
	class ASTToHLSLCompilerModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ASTToHLSLCompiler"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "ASTToHLSLCompiler"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "ProgramCompilerCommon", "RenderCommon", "DynamicModuleDefinition" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "ASTCompiler" }; }
			}
		}
	}
}