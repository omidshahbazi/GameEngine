// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Test
{
	class TestDockableTargetRules : TargetRules
	{
		public override string ModuleName
		{
			get { return "TestDockable"; }
		}

		public TestDockableTargetRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			RequiredModuleNames.Add("DeferredPipeline");

			RequiredModuleNames.Add("ASTToGLSLCompiler");
			RequiredModuleNames.Add("GLSLCompiler");
			RequiredModuleNames.Add("GLSLIntrinsic");
			RequiredModuleNames.Add("OpenGLRenderDevice");
			RequiredModuleNames.Add("VulkanRenderDevice");

			if (OperatingSystem == OperatingSystems.Windows)
			{
				RequiredModuleNames.Add("ASTToHLSLCompiler");
				RequiredModuleNames.Add("HLSLCompiler");
				RequiredModuleNames.Add("HLSLIntrinsic");
				RequiredModuleNames.Add("DirectX12RenderDevice");
			}
		}
	}
}