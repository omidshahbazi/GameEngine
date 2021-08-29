// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class FontSystemModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "FontSystem"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "FontSystem"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "MemoryManagement", "Rendering", "ResourceAssetParser", "Platform" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Containers", "MathContainers", "ResourceCommon" }; }
			}
		}
	}
}