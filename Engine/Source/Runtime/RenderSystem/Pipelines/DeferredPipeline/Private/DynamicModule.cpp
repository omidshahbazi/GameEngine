// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DynamicModuleDefinition\IDynamicModule.h>
#include <DeferredPipeline\Private\DeferredRendering.h>

namespace Engine
{
	using namespace DynamicModuleDefinition;

	namespace DeferredPipeline
	{
		namespace Private
		{
			class DynamicModule : public IDynamicModule
			{
				virtual void* Load(void) override
				{
					return &m_Pipeline;
				}

				virtual bool Unload(void) override
				{
					return true;
				}

			private:
				DeferredRendering m_Pipeline;
			};

			DEFINE_DYNAMIC_MODULE_ENTRY_POINT(DynamicModule);
		}
	}
}