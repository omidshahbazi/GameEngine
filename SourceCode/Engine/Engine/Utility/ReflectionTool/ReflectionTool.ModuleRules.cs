// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class ReflectionToolModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "ReflectionTool"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "ReflectionTool"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.Executable; }
			}

			public override string[] PrivateDependencyModuleNames
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