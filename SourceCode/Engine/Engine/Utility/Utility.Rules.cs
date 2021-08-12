// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class UtilityRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "Utility"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "Utility"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] DependencyModulesName
			{
				get { return new string[] { "Containers", "Platform", "MemoryManagement", "Debugging" }; }
			}
		}
	}
}