// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class ThreadingRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "Threading"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "Threading"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModulesName
			{
				get { return new string[] { "Common", "Platform", "Containers", "Debugging" }; }
			}
		}
	}
}