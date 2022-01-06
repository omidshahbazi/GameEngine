// Copyright 2016-2020 ?????????????. All Rights Reserved.
namespace Engine.Runtime.RenderSystem.RenderDevice
{
	class DirectX12RenderDeviceModuleRules : ModuleRules
	{
		public override string Name
		{
			get { return "DirectX12RenderDevice"; }
		}

		public override UseTypes UseType
		{
			get { return UseTypes.DynamicLibrary; }
		}

		public DirectX12RenderDeviceModuleRules(Configurations Configuration, Platforms Platform) :
			base(Configuration, Platform)
		{
			PrivateDependencyModuleNames.Add("Common");
			PrivateDependencyModuleNames.Add("Containers");
			PrivateDependencyModuleNames.Add("Debugging");
			PrivateDependencyModuleNames.Add("RenderDevice");
			PrivateDependencyModuleNames.Add("RenderCommon");
			PrivateDependencyModuleNames.Add("MemoryManagement");
			PrivateDependencyModuleNames.Add("DynamicModuleDefinition");

			DependencyStaticLibraries.Add("d3d12.lib");
			DependencyStaticLibraries.Add("dxgi.lib");
			DependencyStaticLibraries.Add("d3dcompiler.lib");
		}
	}
}