// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERING_MANAGER_H
#define RENDERING_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <Rendering\DeviceInterface.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		class RENDERING_API RenderingManager
		{
			SINGLETON_DECLARATION(RenderingManager)

		public:
			typedef Vector<DeviceInterface*> DeviceVector;

		private:
			RenderingManager(void);
			~RenderingManager(void);

		public:
			DeviceInterface *CreateDevice(DeviceInterface::Type Type);

			void BeginRender(void);
			
			void EndRender(void);

			DeviceInterface *GetActiveDevice(void) const
			{
				return m_ActiveDevice;
			}

		private:
			DeviceVector m_Devices;
			DeviceInterface *m_ActiveDevice;
		};
	}
}

#endif