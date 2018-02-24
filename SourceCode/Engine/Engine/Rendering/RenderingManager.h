// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERING_MANAGER_H
#define RENDERING_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <Rendering\DeviceInterfarce.h>

namespace Engine
{
	namespace Rendering
	{
		class RENDERING_API RenderingManager
		{
			SINGLETON_DEFINITION(RenderingManager)

		public:
			~RenderingManager(void);

			DeviceInterfarce *CreateDevice(DeviceInterfarce::Type Type);

		private:
			DeviceInterfarce *m_Devices;
		};
	}
}

#endif