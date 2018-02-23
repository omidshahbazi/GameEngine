// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\RenderingManager.h>
#include <Rendering\Private\Allocators.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;

		SINGLETON_DECLARATION(RenderingManager)

		DeviceInterfarce *RenderingManager::CreateDevice(DeviceInterfarce::Type Type)
		{
			DeviceInterfarce *device = reinterpret_cast<DeviceInterfarce*>(AllocateMemory(&Allocators::RenderingSystemAllocator, sizeof(DeviceInterfarce)));
			new (device) DeviceInterfarce(Type);
			return device;
		}
	}
}
