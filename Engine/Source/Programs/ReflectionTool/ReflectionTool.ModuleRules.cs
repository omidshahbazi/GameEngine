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

		public ReflectionToolModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("Allocators");
			PrivateDependencyModuleNames.Add("Containers");
			PrivateDependencyModuleNames.Add("Reflection");
			PrivateDependencyModuleNames.Add("FileUtility");
			PrivateDependencyModuleNames.Add("Lexer");
			PrivateDependencyModuleNames.Add("EntryPointUtility");
			PrivateDependencyModuleNames.Add("MemoryManagement");
			PrivateDependencyModuleNames.Add("ReflectionGenerator");

			IconPath = "Engine/Contents/Icon.png";
		}
	}
}