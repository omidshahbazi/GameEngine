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
	using namespace Containers;
	using namespace Platform;
	using namespace RenderDevice;

	namespace DirectX12RenderDevice
	{
		namespace Private
		{
			class DirectX12CommandBuffer;

			struct ProgramInfos : public IDevice::CompiledShaders
			{
			public:
				ID3D12RootSignature* RootSignature;
				Map<uint32, ID3D12PipelineState*> Pipelines;
				WString PipelineName;
			};

			struct ResourceInfo
			{
			public:
				HeapAllocator::ResourceHandle Resource;
				DescriptorViewAllocator::ViewHandle View;
				D3D12_RESOURCE_STATES State;
			};

			struct BufferInfo : public ResourceInfo
			{
			public:
				uint32 Size;
				uint32 Stride;
				GPUBufferTypes Type;
				bool IsIntermediate;
			};

			struct TextureResourceInfo : public BufferInfo
			{
			public:
				D3D12_SAMPLER_DESC SamplerDescription;
				DescriptorViewAllocator::ViewHandle SamplerView;
				DXGI_FORMAT Format;
				Vector2I Dimension;
			};

			struct ViewInfo : public TextureResourceInfo
			{
			public:
				DescriptorViewAllocator::ViewHandle TargetView;
				AttachmentPoints Point;
			};

			struct RenderTargetInfos
			{
			public:
				typedef Vector<ViewInfo> ViewList;

				ViewList Views;
			};

			struct MeshBufferInfo : public BufferInfo
			{
			public:
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
				INLINE ViewInfo* GetSwapChainView(void)
				{
					return &Views[CurrentViewIndex];
				}

				PlatformWindow::WindowHandle WindowHandle;
				DirectX12CommandBuffer* CommandBuffer;
				IDXGISwapChain4* SwapChain;

				bool Initialized;

				ViewInfo Views[MAX_BACK_BUFFER_COUNT];
				uint8 ViewCount;
				uint8 CurrentViewIndex;
				ViewInfo IntermediateViews[2];
			};

			typedef Map<ResourceHandle, RenderContextInfo*> RenderContextMap;

			INLINE D3D12_COMPARISON_FUNC GetComparisonFunction(TestFunctions TestFunction)
			{
				switch (TestFunction)
				{
				case TestFunctions::Never:
					return D3D12_COMPARISON_FUNC_NEVER;

				case TestFunctions::Less:
					return D3D12_COMPARISON_FUNC_LESS;

				case TestFunctions::LessEqual:
					return D3D12_COMPARISON_FUNC_LESS_EQUAL;

				case TestFunctions::Equal:
					return D3D12_COMPARISON_FUNC_EQUAL;

				case TestFunctions::NotEqual:
					return D3D12_COMPARISON_FUNC_NOT_EQUAL;

				case TestFunctions::GreaterEqual:
					return D3D12_COMPARISON_FUNC_GREATER_EQUAL;

				case TestFunctions::Greater:
					return D3D12_COMPARISON_FUNC_GREATER;

				case TestFunctions::Always:
					return D3D12_COMPARISON_FUNC_ALWAYS;
				}
			}
		}
	}
}

#endif