// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Test
{
	class TestVulkanTargetRules : TargetRules
	{
		public override string ModuleName
		{
			get { return "TestVulkan"; }
		}

		public TestVulkanTargetRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			RequiredModuleNames.Add("DeferredPipeline");

			RequiredModuleNames.Add("ASTToGLSLCompiler");
			RequiredModuleNames.Add("GLSLCompiler");
			RequiredModuleNames.Add("GLSLIntrinsic");
			RequiredModuleNames.Add("VulkanRenderDevice");
		}
	}
}