// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\RenderManager.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace RenderCommon::Private;

	namespace RenderSystem
	{
		using namespace Private;

		SINGLETON_DEFINITION(RenderManager);

		RenderManager::RenderManager(void) :
			m_Device(nullptr)
		{
			RenderSystemAllocators::Create();
		}

		RenderManager::~RenderManager(void)
		{
			if (m_Device != nullptr)
				RenderSystemAllocators::RenderSystemAllocator_Deallocate(m_Device);
		}

		DeviceInterface* RenderManager::CreateDevice(DeviceTypes DeviceType)
		{
			CoreDebugAssert(Categories::RenderSystem, m_Device == nullptr, "Device already created");

			DeviceInterface* device = RenderSystemAllocators::RenderSystemAllocator_Allocate<DeviceInterface>();
			ConstructMacro(DeviceInterface, device, DeviceType);

			m_Device = device;

			return device;
		}
	}
}