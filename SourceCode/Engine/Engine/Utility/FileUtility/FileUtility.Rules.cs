// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class FileUtilityRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "FileUtility"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "FileUtility"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModulesName
			{
				get { return new string[] { "Common", "Containers", "Platform", "MemoryManagement" }; }
			}
		}
	}
}