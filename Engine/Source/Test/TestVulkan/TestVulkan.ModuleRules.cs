// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Test
{
    class TestVulkanModuleRules : ModuleRules
    {
        public override string Name
        {
            get { return "TestVulkan"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.Executable; }
		}

		public TestVulkanModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("RenderSystem");
			PrivateDependencyModuleNames.Add("Allocators");
			PrivateDependencyModuleNames.Add("DynamicModuleSystem");
			PrivateDependencyModuleNames.Add("FileUtility");

			GenerateRenderDocSettings = true;
		}
    }
}