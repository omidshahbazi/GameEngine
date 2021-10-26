// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Runtime
{
	class DebuggingModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Debugging"; }
		}

		public override LibraryUseTypes LibraryUseType
		{
			get { return LibraryUseTypes.DynamicLibrary; }
		}

		public DebuggingModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Platform");
			PrivateDependencyModuleNames.Add("FileUtility");
			PrivateDependencyModuleNames.Add("MemoryManagement");

			PublicDependencyModuleNames.Add("Common");
			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("Threading");
		}
	}
}