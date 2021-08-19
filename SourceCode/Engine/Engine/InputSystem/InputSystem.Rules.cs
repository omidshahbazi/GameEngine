// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class InputSystemRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "InputSystem"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "InputSystem"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] DependencyModulesName
			{
				get { return new string[] { "MemoryManagement", "Platform", "Rendering", "WindowUtility" }; }
			}
		}
	}
}