// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler
{
	class ProgramCompilerCommonModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ProgramCompilerCommon"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ProgramCompilerCommonModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PublicDependencyModuleNames.Add("Common");
		}
	}
}