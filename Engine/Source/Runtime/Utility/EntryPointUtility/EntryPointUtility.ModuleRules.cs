// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.Utility
{
	class EntryPointUtilityModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "EntryPointUtility"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public EntryPointUtilityModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Containers");

			PublicDependencyModuleNames.Add("Debugging");
		}
	}
}