// Copyright 2016-2017 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class VulkanRules : BuildRules
	{
		public override string ModuleName
		{
			get { return "Vulkan"; }
		}

		public class Rule : RuleBase
		{
			public override string TargetName
			{
				get { return "Vulkan"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.UseOnly; }
			}

			public override string RootPath
			{
				get { return "include/"; }
			}

			public override string[] BinariesPath
			{
				get
				{
					return new string[] {
						"dll/VkLayer_api_dump.dll",
						"dll/VkLayer_core_validation.dll",
						"dll/VkLayer_image.dll",
						"dll/VkLayer_object_tracker.dll",
						"dll/VkLayer_parameter_validation.dll",
						"dll/VkLayer_screenshot.dll",
						"dll/VkLayer_swapchain.dll",
						"dll/VkLayer_threading.dll",
						"dll/VkLayer_unique_objects.dll",
						"dll/VkLayer_vktrace_layer.dll"
					};
				}
			}

			public override string[] LibrariesPath
			{
				get
				{
					return new string[] {
						"lib/VkLayer_api_dump.lib",
						"lib/VkLayer_core_validation.lib",
						"lib/VkLayer_image.lib",
						"lib/VkLayer_object_tracker.lib",
						"lib/VkLayer_parameter_validation.lib",
						"lib/VkLayer_screenshot.lib",
						"lib/VkLayer_swapchain.lib",
						"lib/VkLayer_threading.lib",
						"lib/VkLayer_unique_objects.lib",
						"lib/VkLayer_utils.lib",
						"lib/VKstatic.1.lib",
						"lib/vulkan-1.lib"
					};
				}
			}
		}
	}
}