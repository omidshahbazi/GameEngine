// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPEN_GL_COMMANDS_H
#define OPEN_GL_COMMANDS_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Color.h>
#include <MathContainers\MathContainers.h>
#include <RenderCommon\Enums.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace MathContainers;
	using namespace RenderCommon;

	namespace OpenGLRenderDevice
	{
		namespace Private
		{
			enum class CommandTypes
			{
				CopyFromVertexToBuffer = 0,
				CopyFromBufferToVertex,
				CopyFromIndexToBuffer,
				CopyFromBufferToIndex,
				CopyFromTextureToBuffer,
				CopyFromBufferToTexture,
				SetProgram,
				SetProgramConstantBuffer,
				SetProgramTexture,
				SetState,
				SetRenderTarget,
				SetViewport,
				Clear,
				SetMesh,
				DrawIndexed,
				DrawArray,
				BeginEvent,
				EndEvent,
				SetMarker
			};

			struct CopyFromVertexToBufferCommandData
			{
			public:
				ResourceHandle Handle;
				ResourceHandle FromMeshHandle;
				uint32 Size;
			};

			struct CopyFromBufferToVertexCommandData
			{
			public:
				ResourceHandle Handle;
				ResourceHandle ToMeshHandle;
				uint32 Size;
			};

			struct CopyFromIndexToBufferCommandData
			{
			public:
				ResourceHandle Handle;
				ResourceHandle FromMeshHandle;
				uint32 Size;
			};

			struct CopyFromBufferToIndexCommandData
			{
			public:
				ResourceHandle Handle;
				ResourceHandle ToMeshHandle;
				uint32 Size;
			};

			struct CopyFromTextureToBufferCommandData
			{
			public:
				ResourceHandle Handle;
				ResourceHandle FromTextureHandle;
				uint32 Size;
				TextureTypes TextureType;
				Formats TextureFormat;
				uint32 Level;
			};

			struct CopyFromBufferToTextureCommandData
			{
			public:
				ResourceHandle Handle;
				ResourceHandle ToTextureHandle;
				TextureTypes TextureType;
				uint32 Width;
				uint32 Height;
				Formats TextureFormat;
			};

			struct SetProgramCommandData
			{
			public:
				ResourceHandle Handle;
			};

			struct SetProgramConstantBufferCommandData
			{
			public:
				ProgramConstantHandle Handle;
				ResourceHandle Value;
			};

			struct SetProgramTextureCommandData
			{
			public:
				ProgramConstantHandle Handle;
				TextureTypes Type;
				ResourceHandle Value;
			};

			struct SetStateCommandData
			{
			public:
				RenderState State;
			};

			struct SetRenderTargetCommandData
			{
			public:
				ResourceHandle Handle;
			};

			struct SetViewportCommandData
			{
			public:
				Vector2I Position;
				Vector2I Size;
			};

			struct ClearCommandData
			{
			public:
				ClearFlags Flags;
				ColorUI8 Color;
			};

			struct SetMeshCommandData
			{
			public:
				ResourceHandle Handle;
			};

			struct DrawIndexedCommandData
			{
			public:
				PolygonTypes PolygonType;
				uint32 IndexCount;
			};

			struct DrawArrayCommandData
			{
			public:
				PolygonTypes PolygonType;
				uint32 VertexCount;
			};

			struct BeginEventCommandData
			{
			public:
				char8 Label[256];
				uint8 Length;
			};

			struct EndEventCommandData
			{
			};

			struct SetMarkerCommandData
			{
			public:
				char8 Label[256];
				uint8 Length;
			};
		}
	}
}

#endif