// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class DataUtilityModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "DataUtility"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "DataUtility"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Common" }; }
			}
		}
	}
}