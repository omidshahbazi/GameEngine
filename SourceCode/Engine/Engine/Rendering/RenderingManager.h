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

			void BeginRender(void);
			
			void EndRender(void);

			DeviceInterfarce *GetActiveDevice(void) const
			{
				return m_ActiveDevice;
			}

		private:
			DeviceVector m_Devices;
			DeviceInterfarce *m_ActiveDevice;
		};
	}
}

#endif