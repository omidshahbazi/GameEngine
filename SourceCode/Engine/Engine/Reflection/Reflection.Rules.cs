// Copyright 2012-2015 ?????????????. All Rights Reserved.
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
				get { return new string[] { "Containers" }; }
			}
		}
	}
}