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

			public override string[] IncludeModuleNames
			{
				get { return new string[] { "ResourceSystem" }; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Containers", "MemoryManagement", "Rendering", "ResourceAssetParser", "Platform" }; }
			}
		}
	}
}