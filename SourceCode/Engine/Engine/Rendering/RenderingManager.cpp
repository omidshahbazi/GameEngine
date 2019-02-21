// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\RenderingManager.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;

		SINGLETON_DEFINITION(RenderingManager)

		RenderingManager::RenderingManager(void) :
			m_Devices(&RenderingAllocators::RenderingSystemAllocator),
			m_ActiveDevice(nullptr)
		{
		}

		RenderingManager::~RenderingManager(void)
		{
			for each(auto device in m_Devices)
			{
				device->~DeviceInterface();
				DeallocateMemory(&RenderingAllocators::RenderingSystemAllocator, device);
			}
		}

		DeviceInterface *RenderingManager::CreateDevice(DeviceInterface::Type Type)
		{
			DeviceInterface *device = ReinterpretCast(DeviceInterface*, AllocateMemory(&RenderingAllocators::RenderingSystemAllocator, sizeof(DeviceInterface)));
			new (device) DeviceInterface(Type);

			m_Devices.Add(device);

			m_ActiveDevice = device;

			return device;
		}

		void RenderingManager::BeginRender(void)
		{
			for each (auto device in m_Devices)
				device->BeginRender();
		}

		void RenderingManager::EndRender(void)
		{
			for each (auto device in m_Devices)
				device->EndRender();
		}
	}
}