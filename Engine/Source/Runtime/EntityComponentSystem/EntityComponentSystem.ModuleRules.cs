// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class EntityComponentSystemModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "EntityComponentSystem"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public EntityComponentSystemModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PublicDependencyModuleNames.Add("Common");
			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("Allocators");
			PublicDependencyModuleNames.Add("Platform");
			PublicDependencyModuleNames.Add("Debugging");
		}
	}
}