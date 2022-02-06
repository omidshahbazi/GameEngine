// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPEN_GL_COMMAND_BUFFER_H
#define OPEN_GL_COMMAND_BUFFER_H

#include <RenderDevice\ICommandBuffer.h>
#include <RenderCommon\CommandBufferContainer.h>
#include <OpenGLRenderDevice\Private\OpenGLCommands.h>

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
				OpenGLCommandBuffer(OpenGLDevice* Device, Types Type);

				void SetName(cwstr Name) override;

				Types GetType(void) const override
				{
					return m_Type;
				}

				void Clear(void) override
				{
					m_Buffer.Clear();
				}

				void CopyBuffer(ResourceHandle SourceHandle, ResourceHandle DestinationHandle) override;

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

				void BeginEvent(cwstr Label) override;
				void EndEvent(void) override;
				void SetMarker(cwstr Label) override;

				bool Execute(void) override;

			private:
				OpenGLDevice* m_Device;
				Types m_Type;
				char8 m_Name[64];
				uint16 m_NameLength;
				CommandBufferContainer m_Buffer;
			};
		}
	}
}

#endif