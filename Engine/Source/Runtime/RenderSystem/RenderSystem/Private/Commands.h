// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMMANDS_H
#define COMMANDS_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>
#include <Containers\Color.h>
#include <MathContainers\MathContainers.h>
#include <RenderCommon\Enums.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace MathContainers;
	using namespace RenderCommon;

	namespace RenderSystem
	{
		class Texture;
		class RenderTarget;
		class Mesh;
		class Material;

		namespace Private
		{
			enum class CommandTypes
			{
				CopyTexture = 0,
				GenerateMipMap,
				SetRenderTarget,
				SetViewport,
				Clear,
				Draw,
				BeginEvent,
				EndEvent,
				SetMarker,
				WaitForFences,
				SignalFences
			};

			struct CopyTextureCommandData
			{
			public:
				const Texture* Source;
				Vector2I SourcePosition;
				Texture* Destination;
				Vector2I DestinationPosition;
				Vector2I Size;
			};

			struct GenerateMipMapCommandData
			{
			public:
				Texture* Texture;
			};

			struct SetRenderTargetCommandData
			{
			public:
				RenderTarget* RenderTarget;
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

			struct DrawCommandData
			{
			public:
				const Mesh* Mesh;
				Matrix4F Model;
				Matrix4F View;
				Matrix4F Projection;
				Matrix4F MVP;
				const Material* Material;
			};

			struct BeginEventCommandData
			{
			public:
				WString Label;
			};

			struct EndEventCommandData
			{
			};

			struct SetMarkerCommandData
			{
			public:
				WString Label;
			};

			struct SignalFencesCommandData
			{
			public:
				CommandBufferFence* const* Fences;
				uint8 Count;
			};

			struct WaitForFencesCommandData
			{
			public:
				CommandBufferFence* const* Fences;
				uint8 Count;
			};
		}
	}
}

#endif