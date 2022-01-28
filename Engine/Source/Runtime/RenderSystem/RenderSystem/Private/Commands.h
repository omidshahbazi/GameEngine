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
				SetRenderTarget = 0,
				SetViewport,
				Clear,
				Draw,
				BeginEvent,
				EndEvent,
				SetMarker
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
				Mesh* Mesh;
				Matrix4F Model;
				Matrix4F View;
				Matrix4F Projection;
				Matrix4F MVP;
				Material* Material;
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
		}
	}
}

#endif