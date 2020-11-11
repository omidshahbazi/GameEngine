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