// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERING_COMMON_H
#define RENDERING_COMMON_H

#include <Containers\Vector.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				class CommandBase;
			}
		}

		using namespace Private::Commands;

		enum class DeviceTypes
		{
			OpenGL,
			DirectX12,
			Vulkan
		};

		const DeviceTypes DEVICE_TYPES[] = { DeviceTypes::OpenGL, DeviceTypes::DirectX12, DeviceTypes::Vulkan };
		const uint8 DEVICE_TYPE_COUNT = _countof(DEVICE_TYPES);

		enum class Formats
		{
			R8 = 0,
			R16,
			R32,
			R16F,
			R32F,
			RG8,
			RG16,
			RG32,
			RG16F,
			RG32F,
			RGB8,
			RGB16,
			RGB32,
			RGB16F,
			RGB32F,
			RGBA8,
			RGBA16,
			RGBA32,
			RGBA16F,
			RGBA32F,
			Depth16,
			Depth24,
			Depth32,
			Depth32F,
			DepthStencil24F,
			DepthStencil32F
		};

		enum class RenderQueues
		{
			Default = 0,
			Geometry,
			Lighting,
			Transparent,
			HUD,
			COUNT
		};

		typedef Vector<CommandBase*> CommandList;
	}
}

#endif