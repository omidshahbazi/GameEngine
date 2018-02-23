// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\RenderingManager.h>
#include <Rendering\Private\Allocators.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;

		SINGLETON_DECLARATION(RenderingManager)

		RenderingManager::~RenderingManager(void)
		{
			m_Devices->~DeviceInterfarce();
			DeallocateMemory(&Allocators::RenderingSystemAllocator, m_Devices);
		}

		DeviceInterfarce *RenderingManager::CreateDevice(DeviceInterfarce::Type Type)
		{
			m_Devices = reinterpret_cast<DeviceInterfarce*>(AllocateMemory(&Allocators::RenderingSystemAllocator, sizeof(DeviceInterfarce)));
			new (m_Devices) DeviceInterfarce(Type);
			return m_Devices;
		}
	}
}