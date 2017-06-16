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

			public override string[] BinariesPath
			{
				get
				{
					return new string[] {
					};
				}
			}

			public override string[] LibrariesPath
			{
				get
				{
					return new string[] {
						"lib/vulkan-1.lib"
					};
				}
			}
		}
	}
}