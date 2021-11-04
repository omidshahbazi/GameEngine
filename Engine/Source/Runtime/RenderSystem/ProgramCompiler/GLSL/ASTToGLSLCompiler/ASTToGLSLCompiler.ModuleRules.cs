// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler.GLSL
{
	class ASTToGLSLCompilerModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ASTToGLSLCompiler"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ASTToGLSLCompilerModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("ASTCompiler");
			PrivateDependencyModuleNames.Add("ProgramCompilerCommon");
			PrivateDependencyModuleNames.Add("RenderDevice");
			PrivateDependencyModuleNames.Add("DynamicModuleDefinition");
		}
	}
}