// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler
{
	class ProgramParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ProgramParser"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public ProgramParserModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Debugging");

			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("Lexer");
			PublicDependencyModuleNames.Add("ProgramCompilerCommon");
			PublicDependencyModuleNames.Add("RenderCommon");
			PublicDependencyModuleNames.Add("MemoryManagement");
		}
	}
}