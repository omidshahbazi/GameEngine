// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.ThirdParty
{
	class VulkanSDKModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "VulkanSDK"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.UseOnly; }
		}

		public VulkanSDKModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			IncludePaths.Add("include/");

			if (Platform == Platforms.x86)
			{
				LibraryPaths.Add("lib/Win32/vulkan-1.lib");
			}
			else
			{
				LibraryPaths.Add("lib/Win64/vulkan-1.lib");
			}
		}
	}
}