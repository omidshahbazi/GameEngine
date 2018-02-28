// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERING_MANAGER_H
#define RENDERING_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <Rendering\DeviceInterfarce.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		class RENDERING_API RenderingManager
		{
			SINGLETON_DEFINITION(RenderingManager)

		public:
			typedef Vector<DeviceInterfarce*> DeviceVector;

		public:
			RenderingManager(void);
			~RenderingManager(void);

			DeviceInterfarce *CreateDevice(DeviceInterfarce::Type Type);

		private:
			DeviceVector m_Devices;
		};
	}
}

#endif