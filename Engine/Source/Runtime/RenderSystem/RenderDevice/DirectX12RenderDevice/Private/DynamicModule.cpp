// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DynamicModuleDefinition\IDynamicModule.h>
#include <DirectX12RenderDevice\Private\DirectX12Device.h>

namespace Engine
{
	using namespace DynamicModuleDefinition;

	namespace DirectX12RenderDevice
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
					return true;
				}

			private:
				DirectX12Device m_Device;
			};

			DEFINE_DYNAMIC_MODULE_ENTRY_POINT(DynamicModule);
		}
	}
}