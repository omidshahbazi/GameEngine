// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef I_COMMAND_BUFFER_H
#define I_COMMAND_BUFFER_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Color.h>
#include <MathContainers\MathContainers.h>
#include <RenderCommon\RenderCommon.h>
#include <RenderCommon\RenderState.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace MathContainers;
	using namespace RenderCommon;

	namespace RenderDevice
	{
		class ICommandBuffer
		{
		public:
			enum class Types
			{
				Graphics = 0,
				Compute,
				Copy
			};

		public:
			virtual ~ICommandBuffer(void)
			{
			}

			virtual void SetName(cwstr Name) = 0;

			virtual Types GetType(void) const = 0;

			virtual void Clear(void) = 0;

			virtual void CopyFromVertexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size) = 0;
			virtual void CopyFromBufferToVertex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size) = 0;
			virtual void CopyFromIndexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size) = 0;
			virtual void CopyFromBufferToIndex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size) = 0;
			virtual void CopyFromTextureToBuffer(ResourceHandle Handle, ResourceHandle FromTextureHandle, uint32 Size, TextureTypes TextureType, Formats TextureFormat, uint32 Level) = 0;
			virtual void CopyFromBufferToTexture(ResourceHandle Handle, ResourceHandle ToTextureHandle, TextureTypes TextureType, uint32 Width, uint32 Height, Formats TextureFormat) = 0;

			//virtual void GenerateTextureMipMap(ResourceHandle Handle, TextureTypes Type) = 0;

			virtual void SetProgram(ResourceHandle Handle) = 0;
			virtual void SetProgramConstantBuffer(ProgramConstantHandle Handle, ResourceHandle Value) = 0;
			virtual void SetProgramTexture(ProgramConstantHandle Handle, TextureTypes Type, ResourceHandle Value) = 0;
			virtual void SetState(const RenderState& State) = 0;

			virtual void SetRenderTarget(ResourceHandle Handle) = 0;
			virtual void SetViewport(const Vector2I& Position, const Vector2I& Size) = 0;
			virtual void Clear(ClearFlags Flags, const ColorUI8& Color) = 0;

			virtual void SetMesh(ResourceHandle Handle) = 0;

			virtual void DrawIndexed(PolygonTypes PolygonType, uint32 IndexCount) = 0;
			virtual void DrawArray(PolygonTypes PolygonType, uint32 VertexCount) = 0;

			virtual void BeginEvent(cwstr Label) = 0;
			virtual void EndEvent(void) = 0;
			virtual void SetMarker(cwstr Label) = 0;

			virtual bool Execute(void) = 0;
		};
	}
}

#endif