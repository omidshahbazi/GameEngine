// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Frontend
{
	class DirectX12RenderDeviceModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "DirectX12RenderDevice"; }
		}

		public class BuildRules : BuildRulesBase
		{
			public override string TargetName
			{
				get { return "DirectX12RenderDevice"; }
			}

			public override LibraryUseTypes LibraryUseType
			{
				get { return LibraryUseTypes.DynamicLibrary; }
			}

			public override string[] PrivateDependencyModuleNames
			{
				get { return new string[] { "Common", "Containers", "Debugging", "Containers", "RenderDevice", "RenderCommon", "MemoryManagement", "DynamicModuleDefinition", "DataUtility" }; }
			}

			public override string[] DependencyStaticLibraries
			{
				get { return new string[] { "d3d12.lib", "dxgi.lib", "d3dcompiler.lib" }; }
			}
		}
	}
}