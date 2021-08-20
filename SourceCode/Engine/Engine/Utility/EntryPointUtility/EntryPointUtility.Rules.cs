// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class EntryPointUtilityRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "EntryPointUtility"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "EntryPointUtility"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModulesName
			{
				get { return new string[] { "Containers" }; }
			}
		}
	}
}