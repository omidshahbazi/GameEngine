// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class EntryPointUtilityModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "EntryPointUtility"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "EntryPointUtility"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Containers" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Debugging" }; }
			}
		}
	}
}