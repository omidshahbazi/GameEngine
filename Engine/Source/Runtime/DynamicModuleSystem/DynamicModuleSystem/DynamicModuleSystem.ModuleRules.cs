// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.DynamicModuleSystem
{
	class DynamicModuleSystemModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "DynamicModuleSystem"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public DynamicModuleSystemModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("FileUtility");

			PublicDependencyModuleNames.Add("DynamicModuleDefinition");
			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("MemoryManagement");
			PublicDependencyModuleNames.Add("Threading");
			PublicDependencyModuleNames.Add("Platform");
		}
	}
}