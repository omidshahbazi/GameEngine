// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class ReflectionToolRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "ReflectionTool"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "ReflectionTool"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.Executable; }
			}

			public override string[] PrivateDependencyModulesName
			{
				get { return new string[] { "Common", "Debugging", "Containers", "Reflection", "FileUtility", "Lexer", "EntryPointUtility", "MemoryManagement" }; }
			}

			public override Priorities Priority
			{
				get { return Priorities.PreBuildProcess; }
			}
		}
	}
}