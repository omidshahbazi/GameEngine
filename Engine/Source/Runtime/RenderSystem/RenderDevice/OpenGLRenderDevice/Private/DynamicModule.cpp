// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DynamicModuleDefinition\IDynamicModule.h>
#include <OpenGLRenderDevice\Private\OpenGLDevice.h>

namespace Engine
{
	using namespace DynamicModuleDefinition;

	namespace OpenGLRenderDevice
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
				OpenGLDevice m_Device;
			};

			DEFINE_DYNAMIC_MODULE_ENTRY_POINT(DynamicModule);
		}
	}
}