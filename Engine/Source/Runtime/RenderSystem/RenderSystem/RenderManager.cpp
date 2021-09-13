// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\RenderManager.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>

namespace Engine
{
	using namespace RenderCommon::Private;

	namespace RenderSystem
	{
		using namespace Private;

		SINGLETON_DEFINITION(RenderManager);

		RenderManager::RenderManager(void) :
			m_ActiveDevice(nullptr)
		{
			RenderSystemAllocators::Create();

			m_Devices = DeviceList(RenderSystemAllocators::RenderSystemAllocator);
		}

		RenderManager::~RenderManager(void)
		{
			for (auto device : m_Devices)
				RenderSystemAllocators::RenderSystemAllocator_Deallocate(device);
		}

		DeviceInterface* RenderManager::CreateDevice(DeviceTypes DeviceType)
		{
			DeviceInterface* device = RenderSystemAllocators::RenderSystemAllocator_Allocate<DeviceInterface>();
			ConstructMacro(DeviceInterface, device, DeviceType);

			m_Devices.Add(device);

			m_ActiveDevice = device;

			return device;
		}
	}
}