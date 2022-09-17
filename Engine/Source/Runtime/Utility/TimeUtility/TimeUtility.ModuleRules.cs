// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.Utility
{
	class TimeUtilityModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "TimeUtility"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public TimeUtilityModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("Platform");
		}
	}
}