// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.Utility
{
	class TimeUtilityModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "TimeUtility"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "TimeUtility"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Common", "Platform" }; }
			}
		}
	}
}