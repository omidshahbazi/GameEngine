// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include <Containers\Map.h>
#include <RenderDevice\IDevice.h>

namespace Engine
{
	using namespace Platform;
	using namespace RenderDevice;

	namespace VulkanRenderDevice
	{
		namespace Private
		{
			class VulkanDevice : public IDevice
			{
			public:
				VulkanDevice(void);
				~VulkanDevice(void);

				bool Initialize(void) override;
				bool Uninitialize(void) override;

				cstr GetVersion(void) override;
				cstr GetVendorName(void) override;
				cstr GetRendererName(void) override;
				cstr GetShadingLanguageVersion(void) override;

				bool CreateContext(PlatformWindow::WindowHandle WindowHandle, ResourceHandle& Handle) override;
				bool DestroyContext(ResourceHandle Handle) override;
				bool SetContext(ResourceHandle Handle) override;
				bool SwapBuffers(void) override;

				bool CreateBuffer(GPUBufferTypes Type, uint32 Size, ResourceHandle& Handle) override;
				bool DestroyBuffer(ResourceHandle Handle) override;
				bool LockBuffer(ResourceHandle Handle, GPUBufferAccess Access, byte** Buffer) override;
				bool UnlockBuffer(ResourceHandle Handle) override;

				bool CreateProgram(const CompiledShaders* Shaders, ResourceHandle& Handle, cstr* ErrorMessage) override;
				bool DestroyProgram(ResourceHandle Handle) override;

				bool CreateTexture(const TextureInfo* Info, ResourceHandle& Handle) override;
				bool DestroyTexture(ResourceHandle Handle) override;
				bool GetTextureFootprint(ResourceHandle Handle, BufferFootprintInfo& Footprint) override;
				bool SetTextureVerticalWrapping(ResourceHandle Handle, TextureWrapModes Mode) override;
				bool SetTextureHorizontalWrapping(ResourceHandle Handle, TextureWrapModes Mode) override;
				bool SetTextureMinifyFilter(ResourceHandle Handle, TextureMinifyFilters Filter) override;
				bool SetTextureMagnifyFilter(ResourceHandle Handle, TextureMagnfyFilters Filter) override;

				bool CreateRenderTarget(const RenderTargetInfo* Info, ResourceHandle& Handle, TextureList& Textures) override;
				bool DestroyRenderTarget(ResourceHandle Handle) override;

				bool CreateMesh(const SubMeshInfo* Info, ResourceHandle& Handle) override;
				bool DestroyMesh(ResourceHandle Handle) override;

				bool CreateCommandBuffer(ICommandBuffer*& Buffer) override;
				bool DestroyCommandBuffers(ICommandBuffer** Buffers, uint8 Count) override;
				bool SubmitCommandBuffers(ICommandBuffer* const* Buffers, uint8 Count) override;

				bool CreateFence(IFence*& Fence) override;
				bool DestroyFences(IFence** Fence, uint8 Count) override;

				bool SetResourceName(ResourceHandle Handle, ResourceTypes Type, cwstr Name) override;
				bool SetDebugCallback(DebugFunction Callback) override;

				DebugFunction GetDebugCallback(void) const
				{
					return m_DebugCallback;
				}

			private:
				bool m_Initialized;

				ColorUI8 m_ClearColor;
				RenderState m_State;

				ResourceHandle m_LastFrameBuffer;

				DebugFunction m_DebugCallback;

				// Inherited via IDevice
			};
		}
	}
}

#endif