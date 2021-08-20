// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class MathematicsModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Mathematics"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "Mathematics"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PublicDependencyModulesName
			{
				get { return new string[] { "Common" }; }
			}
		}
	}
}