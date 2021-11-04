// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.Utility
{
	class WindowUtilityModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "WindowUtility"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public WindowUtilityModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("Containers");
			PrivateDependencyModuleNames.Add("MathContainers");
			PrivateDependencyModuleNames.Add("Platform");
			PrivateDependencyModuleNames.Add("Debugging");
		}
	}
}