// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagement\Singleton.h>
#include <Rendering\DeviceInterfarce.h>

#ifndef RENDERING_MANAGER_H
#define RENDERING_MANAGER_H

namespace Engine
{
	namespace Rendering
	{
		class RENDERING_API RenderingManager
		{
			SINGLETON_DEFINITION(RenderingManager)

		public:
			DeviceInterfarce *CreateDevice(DeviceInterfarce::Type Type);
		};
	}
}

#endif