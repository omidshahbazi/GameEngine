// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class MathContainersRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "MathContainers"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "MathContainers"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] DependencyModulesName
			{
				get { return new string[] { "Common", "Platform" }; }
			}

			//public override string[] PreprocessorDefinitions
			//{
			//	get { return new string[] { "LEFT_HAND_MATRIX" }; }
			//}

			public override bool GenerateWrapper
			{
				get { return true; }
			}
		}
	}
}