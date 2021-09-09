// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class RenderSystemModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "RenderSystem"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "RenderSystem"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "RenderCommon", "MemoryManagement", "Debugging", "Threading", "DataUtility", "TimeUtility", "Platform", "DynamicModuleSystem" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Common", "Containers", "MathContainers", "RenderDevice", "ResourceCommon", "ProgramCompiler", "ProgramCompilerCommon", "WindowUtility" }; }
			}
		}
	}
}