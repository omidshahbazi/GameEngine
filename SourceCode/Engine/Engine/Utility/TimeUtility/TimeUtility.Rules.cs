// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class TimeUtilityRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "TimeUtility"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "TimeUtility"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] DependencyModulesName
			{
				get { return new string[] { "Common", "Platform" }; }
			}
		}
	}
}