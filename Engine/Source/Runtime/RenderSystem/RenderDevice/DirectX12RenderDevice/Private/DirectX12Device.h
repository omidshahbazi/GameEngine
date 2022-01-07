// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DIRECTX_12_DEVICE_H
#define DIRECTX_12_DEVICE_H

#include <RenderDevice\IDevice.h>
#include <RenderDevice\Private\NativeCommandBufferPool.h>
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

			class DirectX12Device : public IDevice
			{
			private:
				typedef NativeCommandBufferPool<DirectX12CommandBuffer> CommandBufferPool;

				class AsyncCommandBufferList : public Vector<ICommandBuffer*>
				{
				public:
					AsyncCommandBufferList(AllocatorBase* Allocator) :
						Vector<ICommandBuffer*>(Allocator)
					{
					}

					INLINE void Add(ICommandBuffer* Value)
					{
						ScopeGaurd gaurd(m_Lock);

						Vector<ICommandBuffer*>::Add(Value);
					}

				private:
					SpinLock m_Lock;
				};

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
				bool SetContextSize(const Vector2I& Size) override;

				bool SetResourceName(ResourceHandle Handle, ResourceTypes Type, cwstr Name) override;

				bool CreateBuffer(ResourceHandle& Handle) override;
				bool DestroyBuffer(ResourceHandle Handle) override;
				bool LockBuffer(ResourceHandle Handle, GPUBufferTypes Type, GPUBufferAccess Access, byte** Buffer) override;
				bool UnlockBuffer(ResourceHandle Handle, GPUBufferTypes Type) override;
				bool InitializeConstantBuffer(ResourceHandle Handle, const byte* Data, uint32 Size) override;
				bool CopyFromVertexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size) override;
				bool CopyFromBufferToVertex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size) override;
				bool CopyFromIndexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size) override;
				bool CopyFromBufferToIndex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size) override;
				bool CopyFromTextureToBuffer(ResourceHandle Handle, ResourceHandle FromTextureHandle, uint32 Size, TextureTypes TextureType, Formats TextureFormat, uint32 Level) override;
				bool CopyFromBufferToTexture(ResourceHandle Handle, ResourceHandle ToTextureHandle, TextureTypes TextureType, uint32 Width, uint32 Height, Formats TextureFormat) override;

				bool CreateProgram(const CompiledShaders* Shaders, ResourceHandle& Handle, cstr* ErrorMessage) override;
				bool DestroyProgram(ResourceHandle Handle) override;

				bool CreateTexture(const TextureInfo* Info, ResourceHandle& Handle) override;
				bool DestroyTexture(ResourceHandle Handle) override;
				bool SetTextureVerticalWrapping(ResourceHandle Handle, TextureTypes Type, TextureWrapModes Mode) override;
				bool SetTextureHorizontalWrapping(ResourceHandle Handle, TextureTypes Type, TextureWrapModes Mode) override;
				bool SetTextureMinifyFilter(ResourceHandle Handle, TextureTypes Type, TextureMinifyFilters Filter) override;
				bool SetTextureMagnifyFilter(ResourceHandle Handle, TextureTypes Type, TextureMagnfyFilters Filter) override;
				bool GenerateTextureMipMap(ResourceHandle Handle, TextureTypes Type) override;

				bool CreateRenderTarget(const RenderTargetInfo* Info, ResourceHandle& Handle, TextureList& Textures) override;
				bool DestroyRenderTarget(ResourceHandle Handle) override;

				bool CreateMesh(const SubMeshInfo* Info, ResourceHandle& Handle) override;
				bool DestroyMesh(ResourceHandle Handle) override;

				bool CreateCommandBuffer(ICommandBuffer::Types Type, ICommandBuffer*& Buffer) override;
				bool DestroyCommandBuffer(ICommandBuffer* Buffer) override;
				bool SubmitCommandBuffer(ICommandBuffer* const* Buffers, uint16 Count) override;
				bool SubmitCommandBufferAsync(ICommandBuffer* const* Buffers, uint16 Count) override;

				bool SwapBuffers(void) override;

				bool SetDebugCallback(DebugFunction Callback) override;
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

				bool CreateIntermediateBuffer(uint32 Size, BufferInfo* Buffer);

			private:
				bool WaitForAsyncCommandBuffers(void);

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

				D3D12_INPUT_ELEMENT_DESC* m_InputLayout;
				uint8 m_InputLayoutCount;

				RenderContextMap m_Contexts;
				ResourceHandle m_CurrentContextHandle;
				RenderContextInfo* m_CurrentContext;

				CommandBufferPool m_CommandBufferPool;
				AsyncCommandBufferList m_AsyncCommandBuffers;

				DebugFunction m_DebugCallback;
			};
		}
	}
}

#endif