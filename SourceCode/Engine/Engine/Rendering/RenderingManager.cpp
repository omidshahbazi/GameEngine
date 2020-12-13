// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\RenderingManager.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;

		SINGLETON_DEFINITION(RenderingManager)

			RenderingManager::RenderingManager(void) :
			m_ActiveDevice(nullptr)
		{
			RenderingAllocators::Create();

			m_Devices = DeviceList(RenderingAllocators::RenderingSystemAllocator);
		}

		RenderingManager::~RenderingManager(void)
		{
			for (auto device : m_Devices)
				RenderingAllocators::RenderingSystemAllocator_Deallocate(device);
		}

		DeviceInterface* RenderingManager::CreateDevice(DeviceTypes DeviceType)
		{
			DeviceInterface* device = RenderingAllocators::RenderingSystemAllocator_Allocate<DeviceInterface>();
			Construct(device, DeviceType);

			m_Devices.Add(device);

			m_ActiveDevice = device;

			return device;
		}
	}
}