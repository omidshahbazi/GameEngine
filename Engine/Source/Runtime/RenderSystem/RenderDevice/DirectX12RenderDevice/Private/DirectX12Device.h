// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DIRECTX_12_DEVICE_H
#define DIRECTX_12_DEVICE_H

#include <RenderDevice\IDevice.h>
#include <RenderDevice\Private\NativeCommandBufferPool.h>
#include <RenderDevice\Private\NativeFencePool.h>
#include <DirectX12RenderDevice\Private\HeapAllocatorsCollection.h>
#include <DirectX12RenderDevice\Private\DescriptorViewAllocator.h>
#include <DirectX12RenderDevice\Private\DirectX12Common.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Platform;
	using namespace RenderDevice;
	using namespace RenderDevice::Private;

	namespace DirectX12RenderDevice
	{
		namespace Private
		{
			class DirectX12CommandBuffer;
			class DirectX12Fence;

			class DirectX12Device : public IDevice
			{
			private:
				typedef NativeCommandBufferPool<DirectX12Device, DirectX12CommandBuffer> CommandBufferPool;
				typedef NativeFencePool<DirectX12Device, DirectX12Fence> FencePool;

			public:
				DirectX12Device(void);
				~DirectX12Device(void);

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
				bool SetDebugCallback(DebugFunction Callback) override
				{
					m_DebugCallback = Callback;

					return true;
				}
				const DebugFunction& GetDebugCallback(void) const
				{
					return m_DebugCallback;
				}

				ID3D12Device5* GetDevice(void)
				{
					return m_Device;
				}

				D3D12_INPUT_ELEMENT_DESC* GetInputLayout(void) const
				{
					return m_InputLayout;
				}

				uint8 GetInputLayoutCount(void) const
				{
					return m_InputLayoutCount;
				}

				RenderContextInfo* GetCurrentContext(void) const
				{
					return m_CurrentContext;
				}

			private:
				bool WaitForAsyncCommandBuffers(void);

				bool CreateBufferInternal(GPUBufferTypes Type, uint32 Size, bool IsIntermediate, BufferInfo* Buffer);

				bool UpdateSwapChainBufferSize(RenderContextInfo* Info, const Vector2I& Size);
				bool DestroySwapChainBuffers(RenderContextInfo* ContextInfo);

				bool AllocateSampler(TextureResourceInfo* Info);

			private:
				bool m_Initialized;

				IDXGIFactory5* m_Factory;
				IDXGIAdapter3* m_Adapter;
				DXGI_ADAPTER_DESC2 m_AdapterDesc;
				ID3D12Device5* m_Device;

				BufferHeapAllocatorsCollection m_BufferHeapAllocator;
				TextureHeapAllocatorsCollection m_TextureHeapAllocator;
				RenderTargetHeapAllocatorsCollection m_RenderTargetHeapAllocator;
				DescriptorViewAllocator m_RenderTargetViewAllocator;
				DescriptorViewAllocator m_DepthStencilViewAllocator;
				DescriptorViewAllocator m_ResourceViewAllocator;
				DescriptorViewAllocator m_SamplerViewAllocator;

				BufferInfo m_UploadBuffer;
				DirectX12CommandBuffer* m_UploadCommandBuffer;

				D3D12_INPUT_ELEMENT_DESC* m_InputLayout;
				uint8 m_InputLayoutCount;

				RenderContextInfo* m_CurrentContext;

				CommandBufferPool m_CommandBufferPool;
				FencePool m_FencePool;

				DebugFunction m_DebugCallback;
			};
		}
	}
}

#endif