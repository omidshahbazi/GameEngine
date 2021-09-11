// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler.GLSL
{
	class ASTToGLSLCompilerModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ASTToGLSLCompiler"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "ASTToGLSLCompiler"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "ASTCompiler", "ProgramCompilerCommon", "RenderDevice", "DynamicModuleDefinition" }; }
			}
		}
	}
}