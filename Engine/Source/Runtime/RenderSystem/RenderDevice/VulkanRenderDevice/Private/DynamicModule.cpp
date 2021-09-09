// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DynamicModuleDefinition\IDynamicModule.h>
#include <VulkanRenderDevice\Private\VulkanDevice.h>

namespace Engine
{
	using namespace DynamicModuleDefinition;

	namespace VulkanRenderDevice
	{
		namespace Private
		{
			class DynamicModule : public IDynamicModule
			{
				virtual void* Load(void) override
				{
					return &m_Device;
				}

				virtual bool Unload(void) override
				{
					return m_Device.Uninitialize();
				}

			private:
				VulkanDevice m_Device;
			};

			DEFINE_DYNAMIC_MODULE_ENTRY_POINT(DynamicModule);
		}
	}
}