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
				DirectX12CommandBuffer(DirectX12Device* Device);
				~DirectX12CommandBuffer(void);

				void SetName(cwstr Name) override;

				void Clear(void) override;

				void CopyBuffer(ResourceHandle SourceHandle, ResourceHandle DestinationHandle) override;
				void CopyTexture(ResourceHandle SourceHandle, const Vector2I& SourcePosition, ResourceHandle DestinationHandle, const Vector2I& DestinationPosition, const Vector2I& Size) override;

				void GenerateMipMap(ResourceHandle Handle) override;

				void SetRenderTarget(ResourceHandle Handle) override;
				void SetViewport(const Vector2I& Position, const Vector2I& Size) override;
				void Clear(ClearFlags Flags, const ColorUI8& Color) override;

				void SetState(const RenderState& State) override;

				void SetProgram(ResourceHandle Handle) override;
				void SetProgramConstantBuffer(ProgramConstantHandle Handle, ResourceHandle Value) override;
				void SetProgramTexture(ProgramConstantHandle Handle, ResourceHandle Value) override;

				void SetMesh(ResourceHandle Handle) override;

				void DrawIndexed(PolygonTypes PolygonType, uint32 IndexCount) override;
				void DrawArray(PolygonTypes PolygonType, uint32 VertexCount) override;

				void Dispatch(uint32 ThreadGroupCountX, uint32 ThreadGroupCountY, uint32 ThreadGroupCountZ) override;

				void BeginEvent(cwstr Label) override;
				void EndEvent(void) override;
				void SetMarker(cwstr Label) override;

				void WaitForFences(IFence* const* Fences, uint8 Count) override;
				void SignalFences(IFence* const* Fences, uint8 Count) override;

				bool Execute(void) override;

				void MoveTextureToPresentState(ResourceHandle Handle);

				ID3D12CommandQueue* GetQueue(void) const
				{
					return m_Queue;
				}

			private:
				void FillGraphicsPipelineState(DirectX12Wrapper::PipelineStateObject::GraphicsPipelineStateDesc& Desc);

			private:
				DirectX12Device* m_Device;
				ID3D12Device5* m_NativeDevice;

				ID3D12CommandQueue* m_Queue;
				ID3D12CommandAllocator* m_Allocator;
				ID3D12GraphicsCommandList4* m_List;

#ifdef DEBUG_MODE
				ID3D12DebugCommandList2* m_DebugList;
#endif

				RenderState m_State;

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