// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.Utility
{
	class FileUtilityModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "FileUtility"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public FileUtilityModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("Containers");
			PrivateDependencyModuleNames.Add("Platform");
			PrivateDependencyModuleNames.Add("Threading");
			PrivateDependencyModuleNames.Add("Allocators");
		}
	}
}