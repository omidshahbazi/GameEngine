// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler
{
	class ASTCompilerModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ASTCompiler"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public ASTCompilerModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
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