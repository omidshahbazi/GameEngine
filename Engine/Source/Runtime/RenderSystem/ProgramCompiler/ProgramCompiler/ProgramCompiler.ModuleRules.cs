// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ProgramCompiler
{
	class ProgramCompilerModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ProgramCompiler"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ProgramCompilerModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("ProgramParser");
			PrivateDependencyModuleNames.Add("DynamicModuleSystem");
			PrivateDependencyModuleNames.Add("Debugging");
			PrivateDependencyModuleNames.Add("ProgramCompilerCommon");

			PublicDependencyModuleNames.Add("Common");
			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("RenderCommon");
			PublicDependencyModuleNames.Add("RenderDevice");
			PublicDependencyModuleNames.Add("MemoryManagement");
			PublicDependencyModuleNames.Add("ASTCompiler");
		}
	}
}