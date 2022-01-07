// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPEN_GL_COMMAND_BUFFER_H
#define OPEN_GL_COMMAND_BUFFER_H

#include <RenderDevice\ICommandBuffer.h>
#include <RenderCommon\CommandBufferContainer.h>

namespace Engine
{
	using namespace Platform;
	using namespace RenderDevice;

	namespace OpenGLRenderDevice
	{
		namespace Private
		{
			class OpenGLDevice;

			class OpenGLCommandBuffer : public ICommandBuffer
			{
			public:
				OpenGLCommandBuffer(OpenGLDevice* Device);

				void SetName(cwstr Name) override
				{
				}

				Types GetType(void) const override
				{
					return m_Type;
				}
				void SetType(Types Type)
				{
					m_Type = Type;
				}

				void Clear(void) override
				{
					m_Buffer.Clear();
				}

				void CopyFromVertexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size) override;
				void CopyFromBufferToVertex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size) override;
				void CopyFromIndexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size) override;
				void CopyFromBufferToIndex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size) override;
				void CopyFromTextureToBuffer(ResourceHandle Handle, ResourceHandle FromTextureHandle, uint32 Size, TextureTypes TextureType, Formats TextureFormat, uint32 Level) override;
				void CopyFromBufferToTexture(ResourceHandle Handle, ResourceHandle ToTextureHandle, TextureTypes TextureType, uint32 Width, uint32 Height, Formats TextureFormat) override;

				void SetProgram(ResourceHandle Handle) override;
				void SetProgramConstantBuffer(ProgramConstantHandle Handle, ResourceHandle Value) override;
				void SetProgramTexture(ProgramConstantHandle Handle, TextureTypes Type, ResourceHandle Value) override;
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
				OpenGLDevice* m_Device;
				Types m_Type;
				CommandBufferContainer m_Buffer;
			};
		}
	}
}

#endif