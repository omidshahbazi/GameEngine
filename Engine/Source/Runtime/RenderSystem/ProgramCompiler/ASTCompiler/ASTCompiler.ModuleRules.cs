// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler
{
	class ASTCompilerModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ASTCompiler"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ASTCompilerModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("RenderCommon");
			PrivateDependencyModuleNames.Add("DataUtility");
			PrivateDependencyModuleNames.Add("Debugging");

			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("ProgramParser");
			PublicDependencyModuleNames.Add("ProgramCompilerCommon");
			PublicDependencyModuleNames.Add("MemoryManagement");
			PublicDependencyModuleNames.Add("APIIntrinsic");
		}
	}
}