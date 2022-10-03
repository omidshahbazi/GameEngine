// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.RenderDevice
{
	class VulkanRenderDeviceModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "VulkanRenderDevice"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public VulkanRenderDeviceModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("Containers");
			PrivateDependencyModuleNames.Add("RenderDevice");
			PrivateDependencyModuleNames.Add("DynamicModuleDefinition");
		}
	}
}