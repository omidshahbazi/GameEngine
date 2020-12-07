// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class ReflectionRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "Reflection"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "Reflection"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] DependencyModulesName
			{
				get { return new string[] { "Common", "Containers" }; }
			}
		}
	}
}