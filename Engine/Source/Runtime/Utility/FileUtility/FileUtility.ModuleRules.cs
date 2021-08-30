// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class FileUtilityModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "FileUtility"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "FileUtility"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Common", "Containers", "Platform", "Threading", "MemoryManagement" }; }
			}
		}
	}
}