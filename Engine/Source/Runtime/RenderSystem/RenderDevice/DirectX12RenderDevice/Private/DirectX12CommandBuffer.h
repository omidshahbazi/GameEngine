// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DIRECTX_12_COMMAND_BUFFER_H
#define DIRECTX_12_COMMAND_BUFFER_H

#include <Containers\Map.h>
#include <RenderDevice\ICommandBuffer.h>
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
			class DirectX12CommandBuffer : public ICommandBuffer
			{
			private:
				static const uint8 MAX_DESCRIPTOR_HEAP_COUNT = 8;

			public:
				DirectX12CommandBuffer(void);
				~DirectX12CommandBuffer(void);

				void SetViewport(const Vector2I& Position, const Vector2I& Size) override;

				void SetState(const RenderState& State) override;

				void CopyFromVertexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size) override;
				void CopyFromBufferToVertex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size) override;
				void CopyFromIndexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size) override;
				void CopyFromBufferToIndex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size) override;
				void CopyFromTextureToBuffer(ResourceHandle Handle, ResourceHandle FromTextureHandle, uint32 Size, TextureTypes TextureType, Formats TextureFormat, uint32 Level) override;
				void CopyFromBufferToTexture(ResourceHandle Handle, ResourceHandle ToTextureHandle, TextureTypes TextureType, uint32 Width, uint32 Height, Formats TextureFormat) override;

				void BindProgram(ResourceHandle Handle) override;
				void SetProgramConstantBuffer(ProgramConstantHandle Handle, ResourceHandle Value) override;
				void SetProgramTexture(ProgramConstantHandle Handle, TextureTypes Type, ResourceHandle Value) override;

				void BindRenderTarget(ResourceHandle Handle) override;

				void BindMesh(ResourceHandle Handle) override;

				void Clear(ClearFlags Flags, const ColorUI8& Color) override;

				void DrawIndexed(PolygonTypes PolygonType, uint32 IndexCount) override;
				void DrawArray(PolygonTypes PolygonType, uint32 VertexCount) override;

				void BeginEvent(cwstr Label) override;
				void EndEvent(void) override;
				void SetMarker(cwstr Label) override;
			};
		}
	}
}

#endif