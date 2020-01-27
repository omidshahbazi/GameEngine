// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class WrapperToolRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "WrapperTool"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "WrapperTool"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.Executable; }
			}

			public override string[] DependencyModulesName
			{
				get { return new string[] { "Common", "Debugging", "Utility" }; }
			}

			public override Priorities Priority
			{
				get { return Priorities.PreBuildProcess; }
			}
		}
	}
}