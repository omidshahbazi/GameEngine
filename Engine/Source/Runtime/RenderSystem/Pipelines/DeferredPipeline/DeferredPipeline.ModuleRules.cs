// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.Pipelines
{
	class DeferredPipelineModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "DeferredPipeline"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "DeferredPipeline"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "RenderDevice", "DynamicModuleDefinition" }; }
			}

			public override string[] PublicDependencyModuleNames
			{
				get { return new string[] { "Containers", "RenderSystem" }; }
			}
		}
	}
}