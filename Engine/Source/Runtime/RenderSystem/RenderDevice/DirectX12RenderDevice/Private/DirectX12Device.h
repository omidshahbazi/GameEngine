// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DIRECTX_12_DEVICE_H
#define DIRECTX_12_DEVICE_H

#include <Containers\Map.h>
#include <RenderDevice\IDevice.h>
#include <DirectX12RenderDevice\Private\HeapAllocatorsCollection.h>
#include <DirectX12RenderDevice\Private\DescriptorViewAllocator.h>

namespace Engine
{
	using namespace Platform;
	using namespace RenderDevice;

	namespace DirectX12RenderDevice
	{
		namespace Private
		{
			class DirectX12Device : public IDevice
			{
			private:
				static const uint8 MAX_DESCRIPTOR_HEAP_COUNT = 8;

				struct CommandSet
				{
				public:
					ID3D12CommandQueue* Queue;
					ID3D12CommandAllocator* Allocator;
					ID3D12GraphicsCommandList4* List;

#ifdef DEBUG_MODE
					ID3D12DebugCommandList2* Debug;
#endif

					ID3D12Fence* Fence;
					uint64 FenceValue;
					HANDLE FenceEvent;

					uint8 SkipFrameCount;
				};

				struct ProgramInfos : public CompiledShaders
				{
				public:
					ID3D12RootSignature* RootSignature;
					Map<uint32, ID3D12PipelineState*> Pipelines;
				};

				struct ResourceInfo
				{
				public:
					HeapAllocator::ResourceHandle Resource;
					DescriptorViewAllocator::ViewHandle View;
					D3D12_RESOURCE_STATES State;
				};

				struct TextureResourceInfo : public ResourceInfo
				{
				public:
					D3D12_SAMPLER_DESC SamplerDescription;
					DescriptorViewAllocator::ViewHandle SamplerView;
				};

				struct BufferInfo : public ResourceInfo
				{
				public:
					uint32 Size;
					uint32 Stride;
				};

				struct BoundBuffersInfo
				{
				public:
					ResourceInfo* Resource;
					BufferInfo Buffer;
				};

				struct ViewInfo : public TextureResourceInfo
				{
				public:
					DescriptorViewAllocator::ViewHandle TargetView;
					AttachmentPoints Point;
					DXGI_FORMAT Format;
				};

				struct RenderTargetInfos
				{
				public:
					typedef Vector<ViewInfo> ViewList;

					ViewList Views;
				};

				struct MeshBufferInfo
				{
				public:
					BufferInfo VertexBuffer;
					BufferInfo IndexBuffer;
					VertexLayouts Layout;
				};

				class RenderContextInfo
				{
				public:
					static const uint8 MAX_BACK_BUFFER_COUNT = 3;
					static const uint8 RENDER_TARGET_VIEW_INDEX = 0;
					static const uint8 DEPTH_STENCIL_VIEW_INDEX = 1;

				public:
					INLINE ViewInfo* GetRenderTargetViews(void)
					{
						return &Views[CurrentBackBufferIndex][RENDER_TARGET_VIEW_INDEX];
					}

					INLINE ViewInfo* GetDepthStencilViews(void)
					{
						return &Views[CurrentBackBufferIndex][DEPTH_STENCIL_VIEW_INDEX];
					}

					IDXGISwapChain4* SwapChain;

					ViewInfo Views[MAX_BACK_BUFFER_COUNT][2];
					uint8 BackBufferCount;
					uint8 CurrentBackBufferIndex;
					bool Initialized;
					Vector2I Size;
				};

				typedef Map<ResourceHandle, RenderContextInfo*> RenderContextMap;

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
				bool InitializeConstantBuffer(ResourceHandle Handle, const byte* Data, uint32 Size) override;
				bool LockBuffer(ResourceHandle Handle, GPUBufferTypes Type, GPUBufferAccess Access, byte** Buffer) override;
				bool UnlockBuffer(ResourceHandle Handle, GPUBufferTypes Type) override;

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

				virtual bool CreateCommandBuffer(ICommandBuffer*& Buffer) override;
				virtual bool DestroyCommandBuffer(ICommandBuffer* Buffer) override;
				virtual bool SubmitCommandBuffer(const ICommandBuffer* Buffer) override;

				bool SwapBuffers(void) override;

				bool SetDebugCallback(DebugFunction Callback) override;

				DebugFunction GetDebugCallback(void) const
				{
					return m_DebugCallback;
				}

			private:
				bool AddTransitionResourceBarrier(CommandSet& Set, ResourceInfo* Info, D3D12_RESOURCE_STATES AfterState);

				bool DestroySwapChainBuffers(RenderContextInfo* ContextInfo);

				bool CreateIntermediateBuffer(uint32 Size, BufferInfo* Buffer);

				bool CreateCommandSet(CommandSet& Set, D3D12_COMMAND_LIST_TYPE Type);

				bool DestroyCommandSet(CommandSet& Set);

				bool ExecuteCommands(CommandSet& Set);

				bool WaitForGPU(void);

				bool WaitForGPU(CommandSet& Set);

				bool AllocateSampler(TextureResourceInfo* Info);

				bool CopyBuffer(GPUBufferTypes Type, ResourceInfo* Source, bool SourceIsABuffer, ResourceInfo* Destination, bool DestinationIsABuffer);

				void FillGraphicsPipelineState(const RenderState& State, DirectX12Wrapper::PipelineStateObject::GraphicsPipelineStateDesc& Desc);

				uint32 GetStateHash(void);

			private:
				bool m_Initialized;

				IDXGIFactory5* m_Factory;
				IDXGIAdapter3* m_Adapter;
				DXGI_ADAPTER_DESC2 m_AdapterDesc;
				ID3D12Device5* m_Device;
#ifdef DEBUG_MODE
				ID3D12InfoQueue* m_InfoQueue;
#endif
				CommandSet m_CopyCommandSet;
				CommandSet m_RenderCommandSet;

				BufferHeapAllocatorsCollection m_BufferHeapAllocator;
				TextureHeapAllocatorsCollection m_TextureHeapAllocator;
				RenderTargetHeapAllocatorsCollection m_RenderTargetHeapAllocator;
				DescriptorViewAllocator m_RenderTargetViewAllocator;
				DescriptorViewAllocator m_DepthStencilViewAllocator;
				DescriptorViewAllocator m_ResourceViewAllocator;
				DescriptorViewAllocator m_SamplerViewAllocator;
				BufferInfo m_UploadBuffer;

				D3D12_VIEWPORT m_Viewport;
				ColorUI8 m_ClearColor;
				RenderState m_State;

				D3D12_INPUT_ELEMENT_DESC* m_InputLayout;
				uint8 m_InputLayoutCount;

				RenderContextMap m_Contexts;
				ResourceHandle m_CurrentContextHandle;
				RenderContextInfo* m_CurrentContext;

				RenderTargetInfos* m_CurrentRenderTarget;

				ViewInfo* m_CurrentRenderTargetViews[(uint8)AttachmentPoints::Color8 - (uint8)AttachmentPoints::Color0];
				uint8 m_CurrentRenderTargetViewCount;
				ViewInfo* m_CurrentDepthStencilView;

				ID3D12DescriptorHeap* m_CurrentDescriptorHeaps[MAX_DESCRIPTOR_HEAP_COUNT];
				uint8 m_CurrentDescriptorHeapCount;

				bool m_AnyProgramBound;

				DebugFunction m_DebugCallback;
			};
		}
	}
}

#endif