// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler.HLSL
{
	class HLSLIntrinsicModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "HLSLIntrinsic"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public HLSLIntrinsicModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("APIIntrinsic");
			PrivateDependencyModuleNames.Add("DynamicModuleDefinition");
			PrivateDependencyModuleNames.Add("ProgramCompilerCommon");
			PrivateDependencyModuleNames.Add("ASTToHLSLCompiler");
			PrivateDependencyModuleNames.Add("ProgramParser");
		}
	}
}