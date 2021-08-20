// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class YAMLRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "YAML"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "YAML"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] DependencyModulesName
			{
				get { return new string[] { "Containers", "Lexer" }; }
			}
		}
	}
}