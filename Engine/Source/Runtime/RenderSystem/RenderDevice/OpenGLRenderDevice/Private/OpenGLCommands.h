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
				GenerateMipMap,
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

			struct CopyBufferCommandData
			{
			public:
				ResourceHandle Source;
				ResourceHandle Destination;
			};

			struct GenerateMipMapCommandData
			{
			public:
				TextureBufferInfo* Texture;
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

			struct SetStateCommandData
			{
			public:
				RenderState State;
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