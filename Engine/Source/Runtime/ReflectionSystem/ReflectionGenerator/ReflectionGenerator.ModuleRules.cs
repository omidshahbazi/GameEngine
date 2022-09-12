// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Programs
{
	class ReflectionGeneratorModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ReflectionGenerator"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ReflectionGeneratorModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("FileUtility");
			PrivateDependencyModuleNames.Add("EntryPointUtility");
			PrivateDependencyModuleNames.Add("MemoryManagement");
			PrivateDependencyModuleNames.Add("Reflection");
			PrivateDependencyModuleNames.Add("Lexer");

			PublicDependencyModuleNames.Add("Allocators");
			PublicDependencyModuleNames.Add("Containers");
		}
	}
}