// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class InputSystemModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "InputSystem"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "InputSystem"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Debugging", "MathContainers", "MemoryManagement", "Platform", "RenderSystem", "WindowUtility" }; }
			}
		}
	}
}