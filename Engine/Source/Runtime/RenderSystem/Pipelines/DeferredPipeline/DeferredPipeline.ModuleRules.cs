// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.Pipelines
{
	class DeferredPipelineModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "DeferredPipeline"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public DeferredPipelineModuleRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform) :
			base(OperatingSystem, Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("RenderDevice");
			PrivateDependencyModuleNames.Add("DynamicModuleDefinition");

			PublicDependencyModuleNames.Add("Containers");
			PublicDependencyModuleNames.Add("RenderSystem");
			PublicDependencyModuleNames.Add("BuiltInResources");
		}
	}
}