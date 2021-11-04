// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Programs
{
	class ReflectionToolModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ReflectionTool"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.Executable; }
		}

		public ReflectionToolModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("Debugging");
			PrivateDependencyModuleNames.Add("Containers");
			PrivateDependencyModuleNames.Add("Reflection");
			PrivateDependencyModuleNames.Add("FileUtility");
			PrivateDependencyModuleNames.Add("Lexer");
			PrivateDependencyModuleNames.Add("EntryPointUtility");
			PrivateDependencyModuleNames.Add("MemoryManagement");
		}
	}
}