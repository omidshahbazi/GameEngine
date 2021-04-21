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