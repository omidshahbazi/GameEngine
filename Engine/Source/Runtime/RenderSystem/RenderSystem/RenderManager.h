// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <RenderSystem\DeviceInterface.h>

namespace Engine
{
	using namespace Containers;

	namespace RenderSystem
	{
		class RENDERSYSTEM_API RenderManager
		{
			SINGLETON_DECLARATION(RenderManager)

		public:
			typedef Vector<DeviceInterface*> DeviceList;

		private:
			RenderManager(void);
			~RenderManager(void);

		public:
			DeviceInterface* CreateDevice(DeviceTypes DeviceType);

			DeviceInterface* GetDevice(void) const
			{
				return m_Device;
			}

		private:
			DeviceInterface* m_Device;
		};
	}
}

#endif