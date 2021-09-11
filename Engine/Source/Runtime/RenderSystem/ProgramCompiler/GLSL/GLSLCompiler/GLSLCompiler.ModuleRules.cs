// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler.GLSL
{
	class GLSLForVulkanCompilerModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "GLSLCompiler"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "GLSLCompiler"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Common", "Platform", "ProgramCompilerCommon", "RenderCommon", "DynamicModuleDefinition", "GLSLANG" }; }
			}
		}
	}
}