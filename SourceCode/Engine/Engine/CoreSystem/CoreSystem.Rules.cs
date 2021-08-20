// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class CoreSystemRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "CoreSystem"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "CoreSystem"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModulesName
			{
				get { return new string[] { "FileUtility", "InputSystem", "FontSystem", "GameObjectSystem", "ResourceSystem", "Profiler" }; }
			}

			public override string[] PublicDependencyModulesName
			{
				get { return new string[] { "Common", "MemoryManagement", "WindowUtility", "TimeUtility", "Containers", "Rendering" }; }
			}
		}
	}
}