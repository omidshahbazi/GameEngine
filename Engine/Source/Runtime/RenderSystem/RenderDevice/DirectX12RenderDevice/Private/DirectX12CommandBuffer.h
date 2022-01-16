// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DIRECTX_12_COMMAND_BUFFER_H
#define DIRECTX_12_COMMAND_BUFFER_H

#include <RenderDevice\ICommandBuffer.h>
#include <Containers\Map.h>
#include <DirectX12RenderDevice\Private\HeapAllocatorsCollection.h>
#include <DirectX12RenderDevice\Private\DescriptorViewAllocator.h>
#include <DirectX12RenderDevice\Private\DirectX12Wrapper.h>
#include <DirectX12RenderDevice\Private\DirectX12Common.h>

namespace Engine
{
	using namespace Platform;
	using namespace RenderDevice;

	namespace DirectX12RenderDevice
	{
		namespace Private
		{
			class DirectX12Device;

			class DirectX12CommandBuffer : public ICommandBuffer
			{
			private:
				static const uint8 MAX_DESCRIPTOR_HEAP_COUNT = 8;

			public:
				DirectX12CommandBuffer(DirectX12Device* Device, Types Type);
				~DirectX12CommandBuffer(void);

				void SetName(cwstr Name) override;

				Types GetType(void) const override
				{
					return m_Type;
				}

				void Clear(void) override;

				void CopyBuffer(ResourceHandle SourceHandle, ResourceHandle DestinationHandle) override;

				void SetProgram(ResourceHandle Handle) override;
				void SetProgramConstantBuffer(ProgramConstantHandle Handle, ResourceHandle Value) override;
				void SetProgramTexture(ProgramConstantHandle Handle, ResourceHandle Value) override;
				void SetState(const RenderState& State) override;

				void SetRenderTarget(ResourceHandle Handle) override;
				void SetViewport(const Vector2I& Position, const Vector2I& Size) override;
				void Clear(ClearFlags Flags, const ColorUI8& Color) override;

				void SetMesh(ResourceHandle Handle) override;

				void DrawIndexed(PolygonTypes PolygonType, uint32 IndexCount) override;
				void DrawArray(PolygonTypes PolygonType, uint32 VertexCount) override;

				void BeginEvent(cwstr Label) override;
				void EndEvent(void) override;
				void SetMarker(cwstr Label) override;

				bool Execute(void) override;

			private:
				void FillGraphicsPipelineState(DirectX12Wrapper::PipelineStateObject::GraphicsPipelineStateDesc& Desc);

			private:
				DirectX12Device* m_Device;
				ID3D12Device5* m_NativeDevice;
				Types m_Type;
				ID3D12CommandQueue* m_Queue;
				ID3D12CommandAllocator* m_Allocator;
				ID3D12GraphicsCommandList4* m_List;

#ifdef DEBUG_MODE
				ID3D12DebugCommandList2* m_DebugList;
#endif

				ID3D12Fence* m_Fence;
				uint64 m_FenceValue;
				HANDLE m_FenceEvent;

				D3D12_INPUT_ELEMENT_DESC* m_InputLayout;
				uint8 m_InputLayoutCount;

				RenderState m_State;
				ColorUI8 m_ClearColor;

				RenderTargetInfos* m_CurrentRenderTarget;

				ViewInfo* m_CurrentRenderTargetViews[(uint8)AttachmentPoints::Color8 - (uint8)AttachmentPoints::Color0];
				uint8 m_CurrentRenderTargetViewCount;
				ViewInfo* m_CurrentDepthStencilView;

				ID3D12DescriptorHeap* m_CurrentDescriptorHeaps[MAX_DESCRIPTOR_HEAP_COUNT];
				uint8 m_CurrentDescriptorHeapCount;
			};
		}
	}
}

#endif