// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.Utility
{
	class EntryPointUtilityModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "EntryPointUtility"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public EntryPointUtilityModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Containers");

			PublicDependencyModuleNames.Add("Debugging");
		}
	}
}