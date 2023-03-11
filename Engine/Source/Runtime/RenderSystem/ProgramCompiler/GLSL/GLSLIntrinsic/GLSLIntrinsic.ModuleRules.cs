// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler.GLSL
{
	class GLSLIntrinsicModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "GLSLIntrinsic"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public GLSLIntrinsicModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("APIIntrinsic");
			PrivateDependencyModuleNames.Add("DynamicModuleDefinition");
			PrivateDependencyModuleNames.Add("ASTCompiler");
		}
	}
}