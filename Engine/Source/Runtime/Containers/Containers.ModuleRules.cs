// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class ContainersModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Containers"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public ContainersModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PublicDependencyModuleNames.Add("Common");
			PublicDependencyModuleNames.Add("MemoryManagement");
			PublicDependencyModuleNames.Add("Allocators");
			PublicDependencyModuleNames.Add("Platform");
			PublicDependencyModuleNames.Add("Mathematics");
		}
	}
}