// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.MemoryManagement
{
	class MemoryManagementModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "MemoryManagement"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public MemoryManagementModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PublicDependencyModuleNames.Add("Common");
			PublicDependencyModuleNames.Add("Allocators");
		}
	}
}