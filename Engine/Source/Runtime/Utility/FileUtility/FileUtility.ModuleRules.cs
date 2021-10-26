// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.Utility
{
	class FileUtilityModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "FileUtility"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public FileUtilityModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("Containers");
			PrivateDependencyModuleNames.Add("Platform");
			PrivateDependencyModuleNames.Add("Threading");
			PrivateDependencyModuleNames.Add("MemoryManagement");
		}
	}
}