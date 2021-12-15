// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef I_COMMAND_BUFFER_H
#define I_COMMAND_BUFFER_H

//#include <Containers\Color.h>
//#include <RenderCommon\RenderCommon.h>
//#include <RenderCommon\Enums.h>
//#include <RenderDevice\TextureInfo.h>
//#include <RenderDevice\RenderTargetInfo.h>
//#include <RenderDevice\MeshInfo.h>
//#include <Platform\PlatformWindow.h>
//#include <Common\BitwiseUtils.h>

namespace Engine
{
	//using namespace Common;
	//using namespace Platform;
	//using namespace Containers;
	//using namespace RenderCommon;

	namespace RenderDevice
	{
		class ICommandBuffer
		{
		public:
			virtual ~ICommandBuffer(void)
			{
			}

			virtual bool SetContext(ResourceHandle Handle) = 0;

			virtual bool SetViewport(const Vector2I& Position, const Vector2I& Size) = 0;

			virtual void SetState(const RenderState& State) = 0;

			virtual bool CopyFromVertexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size) = 0;
			virtual bool CopyFromBufferToVertex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size) = 0;
			virtual bool CopyFromIndexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size) = 0;
			virtual bool CopyFromBufferToIndex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size) = 0;
			virtual bool CopyFromTextureToBuffer(ResourceHandle Handle, ResourceHandle FromTextureHandle, uint32 Size, TextureTypes TextureType, Formats TextureFormat, uint32 Level) = 0;
			virtual bool CopyFromBufferToTexture(ResourceHandle Handle, ResourceHandle ToTextureHandle, TextureTypes TextureType, uint32 Width, uint32 Height, Formats TextureFormat) = 0;

			virtual bool BindProgram(ResourceHandle Handle) = 0;
			virtual bool SetProgramConstantBuffer(ProgramConstantHandle Handle, ResourceHandle Value) = 0;
			virtual bool SetProgramTexture(ProgramConstantHandle Handle, TextureTypes Type, ResourceHandle Value) = 0;

			virtual	bool GenerateTextureMipMap(ResourceHandle Handle, TextureTypes Type) = 0;

			virtual bool BindRenderTarget(ResourceHandle Handle) = 0;

			virtual bool BindMesh(ResourceHandle Handle) = 0;

			virtual bool Clear(ClearFlags Flags, const ColorUI8& Color) = 0;

			virtual bool DrawIndexed(PolygonTypes PolygonType, uint32 IndexCount) = 0;
			virtual bool DrawArray(PolygonTypes PolygonType, uint32 VertexCount) = 0;

			virtual bool BeginEvent(cwstr Label) = 0;
			virtual bool EndEvent(void) = 0;
			virtual bool SetMarker(cwstr Label) = 0;
		};
	}
}

#endif