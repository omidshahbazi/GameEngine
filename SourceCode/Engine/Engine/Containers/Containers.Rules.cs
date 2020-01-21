// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class ContainersRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "Containers"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "Containers"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] DependencyModulesName
			{
				get { return new string[] { "Common", "MemoryManagement" }; }
			}

			//public override string[] PreprocessorDefinitions
			//{
			//	get { return new string[] { "LEFT_HAND_MATRIX" }; }
			//}
		}
	}
}