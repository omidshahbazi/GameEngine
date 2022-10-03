// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class ContainersModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Containers"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ContainersModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PublicDependencyModuleNames.Add("Common");
			PublicDependencyModuleNames.Add("MemoryManagement");
			PublicDependencyModuleNames.Add("Allocators");
			PublicDependencyModuleNames.Add("Platform");
			PublicDependencyModuleNames.Add("Mathematics");
		}
	}
}