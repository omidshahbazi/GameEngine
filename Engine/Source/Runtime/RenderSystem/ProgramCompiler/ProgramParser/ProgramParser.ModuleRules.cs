// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.ProgramCompiler
{
	class ProgramParserModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ProgramParser"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ProgramParserModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
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