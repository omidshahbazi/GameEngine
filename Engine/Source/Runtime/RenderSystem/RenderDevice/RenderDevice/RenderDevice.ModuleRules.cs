// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.RenderDevice
{
	class RenderDeviceModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "RenderDevice"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "RenderDevice"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "MathContainers" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Containers", "WindowUtility", "RenderCommon", "ProgramCompilerCommon" }; }
			}
		}
	}
}