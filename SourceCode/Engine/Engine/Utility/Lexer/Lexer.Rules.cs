// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class LexerRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "Lexer"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "Lexer"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModulesName
			{
				get { return new string[] { "Common", "Containers", "Debugging", "Platform" }; }
			}
		}
	}
}