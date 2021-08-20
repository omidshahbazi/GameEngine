// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class WindowUtilityRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "WindowUtility"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "WindowUtility"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModulesName
			{
				get { return new string[] { "Common", "Containers", "MathContainers", "Platform" }; }
			}
		}
	}
}