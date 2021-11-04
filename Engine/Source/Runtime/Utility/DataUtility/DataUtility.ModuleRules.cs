// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.Utility
{
	class DataUtilityModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "DataUtility"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public DataUtilityModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
		}
	}
}