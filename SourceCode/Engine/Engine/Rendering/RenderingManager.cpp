// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\RenderingManager.h>
#include <Rendering\Private\Allocators.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;

		SINGLETON_DECLARATION(RenderingManager)

		RenderingManager::RenderingManager(void) :
			m_Devices(&Allocators::RenderingSystemAllocator),
			m_ActiveDevice(nullptr)
		{
		}

		RenderingManager::~RenderingManager(void)
		{
			for each(auto device in m_Devices)
			{
				device->~DeviceInterfarce();
				DeallocateMemory(&Allocators::RenderingSystemAllocator, device);
			}
		}

		DeviceInterfarce *RenderingManager::CreateDevice(DeviceInterfarce::Type Type)
		{
			DeviceInterfarce *device = ReinterpretCast(DeviceInterfarce*, AllocateMemory(&Allocators::RenderingSystemAllocator, sizeof(DeviceInterfarce)));
			new (device) DeviceInterfarce(Type);

			m_Devices.Add(device);

			m_ActiveDevice = device;

			return device;
		}

		void RenderingManager::Update(void)
		{
			for each (auto device in m_Devices)
				device->Update();
		}
	}
}