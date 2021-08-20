// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class MathContainersModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "MathContainers"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "MathContainers"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PublicDependencyModulesName
			{
				get { return new string[] { "Common", "Platform", "Debugging", "Mathematics" }; }
			}

			//public override string[] PreprocessorDefinitions
			//{
			//	get { return new string[] { "LEFT_HAND_MATRIX" }; }
			//}
		}
	}
}