// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.DynamicModuleSystem
{
	class DynamicModuleSystemModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "DynamicModuleSystem"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public DynamicModuleSystemModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
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