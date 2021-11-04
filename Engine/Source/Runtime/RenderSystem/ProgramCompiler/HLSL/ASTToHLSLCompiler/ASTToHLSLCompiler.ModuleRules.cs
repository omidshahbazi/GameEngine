// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler.HLSL
{
	class ASTToHLSLCompilerModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ASTToHLSLCompiler"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ASTToHLSLCompilerModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("ProgramCompilerCommon");
			PrivateDependencyModuleNames.Add("RenderCommon");
			PrivateDependencyModuleNames.Add("DynamicModuleDefinition");

			PublicDependencyModuleNames.Add("ASTCompiler");
		}
	}
}