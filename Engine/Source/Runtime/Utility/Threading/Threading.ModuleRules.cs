// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.Utility
{
	class ThreadingModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "Threading"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "Threading"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Common", "Platform", "Containers" }; }
			}
		}
	}
}