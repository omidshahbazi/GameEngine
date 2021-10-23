// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem
{
	class RenderCommonModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "RenderCommon"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "RenderCommon"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Allocators", "Containers", "MathContainers", "Platform", "MemoryManagement" }; }
			}
		}
	}
}