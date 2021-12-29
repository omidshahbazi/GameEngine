// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DIRECTX_12_COMMON_H
#define DIRECTX_12_COMMON_H

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
			//				struct CommandSet
			//				{
			//				public:
			//					ID3D12CommandQueue* Queue;
			//					ID3D12CommandAllocator* Allocator;
			//					ID3D12GraphicsCommandList4* List;
			//
			//#ifdef DEBUG_MODE
			//					ID3D12DebugCommandList2* Debug;
			//#endif
			//
			//					ID3D12Fence* Fence;
			//					uint64 FenceValue;
			//					HANDLE FenceEvent;
			//
			//					uint8 SkipFrameCount;
			//				};

			struct ProgramInfos : public IDevice::CompiledShaders
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
		}
	}
}

#endif