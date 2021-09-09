// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class VulkanRenderDeviceModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "VulkanRenderDevice"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "VulkanRenderDevice"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Common", "Containers", "RenderDevice", "DynamicModuleDefinition" }; }
			}
		}
	}
}