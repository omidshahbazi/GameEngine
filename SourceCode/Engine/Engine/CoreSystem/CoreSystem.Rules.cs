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

			public override string[] DependencyModulesName
			{
				get { return new string[] { "MemoryManagement", "Utility", "Rendering", "InputSystem", "FontSystem", "GameObjectSystem", "ResourceSystem", "Profiler" }; }
			}

			public override bool GenerateWrapper
			{
				get { return true; }
			}
		}
	}
}