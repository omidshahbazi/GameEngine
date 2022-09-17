// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler.GLSL
{
	class GLSLCompilerModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "GLSLCompiler"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public GLSLCompilerModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("Platform");
			PrivateDependencyModuleNames.Add("ProgramCompilerCommon");
			PrivateDependencyModuleNames.Add("RenderCommon");
			PrivateDependencyModuleNames.Add("DynamicModuleDefinition");
			PrivateDependencyModuleNames.Add("GLSLANG");
		}
	}
}