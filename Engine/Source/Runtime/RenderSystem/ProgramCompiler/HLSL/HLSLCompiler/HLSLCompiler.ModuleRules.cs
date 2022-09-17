// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler.HLSL
{
	class HLSLCompilerModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "HLSLCompiler"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public HLSLCompilerModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("Platform");
			PrivateDependencyModuleNames.Add("ProgramCompilerCommon");
			PrivateDependencyModuleNames.Add("RenderCommon");
			PrivateDependencyModuleNames.Add("DynamicModuleDefinition");

			DependencyStaticLibraries.Add("d3dcompiler.lib");
		}
	}
}