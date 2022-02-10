// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPEN_GL_COMMANDS_H
#define OPEN_GL_COMMANDS_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Color.h>
#include <MathContainers\MathContainers.h>
#include <RenderCommon\Enums.h>
#include <OpenGLRenderDevice\Private\OpenGLCommon.h>

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
				CopyBuffer = 0,
				CopyTexture,
				GenerateMipMap,
				SetRenderTarget,
				SetViewport,
				Clear,
				SetMesh,
				SetProgram,
				SetProgramConstantBuffer,
				SetProgramTexture,
				SetState,
				DrawIndexed,
				DrawArray,
				BeginEvent,
				EndEvent,
				SetMarker
			};

			struct CopyBufferCommandData
			{
			public:
				BufferInfo* Source;
				BufferInfo* Destination;
			};

			struct CopyTextureCommandData
			{
			public:
				TextureBufferInfo* Source;
				Vector2I SourcePosition;
				TextureBufferInfo* Destination;
				Vector2I DestinationPosition;
				Vector2I Size;
			};

			struct GenerateMipMapCommandData
			{
			public:
				TextureBufferInfo* Texture;
			};

			struct SetRenderTargetCommandData
			{
			public:
				RenderTargetBufferInfo* RenderTarget;
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

			struct SetStateCommandData
			{
			public:
				RenderState State;
			};

			struct SetProgramCommandData
			{
			public:
				ProgramInfo* Program;
			};

			struct SetProgramConstantBufferCommandData
			{
			public:
				ProgramConstantHandle Handle;
				BufferInfo* Value;
			};

			struct SetProgramTextureCommandData
			{
			public:
				ProgramConstantHandle Handle;
				TextureBufferInfo* Value;
			};

			struct SetMeshCommandData
			{
			public:
				MeshBufferInfo* Mesh;
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