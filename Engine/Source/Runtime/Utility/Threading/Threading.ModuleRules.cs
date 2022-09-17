// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.Utility
{
	class ThreadingModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Threading"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public ThreadingModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("Platform");
			PrivateDependencyModuleNames.Add("Containers");
		}
	}
}