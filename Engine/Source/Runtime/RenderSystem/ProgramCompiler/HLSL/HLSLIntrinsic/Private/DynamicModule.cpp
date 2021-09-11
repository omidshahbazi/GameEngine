// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DynamicModuleDefinition\IDynamicModule.h>
#include <HLSLIntrinsic\Private\HLSLIntrinsicProvider.h>

namespace Engine
{
	using namespace DynamicModuleDefinition;

	namespace HLSLIntrinsic
	{
		namespace Private
		{
			class DynamicModule : public IDynamicModule
			{
				virtual void* Load(void) override
				{
					return &m_Provider;
				}

				virtual bool Unload(void) override
				{
					return true;
				}

			private:
				HLSLIntrinsicProvider m_Provider;
			};

			DEFINE_DYNAMIC_MODULE_ENTRY_POINT(DynamicModule);
		}
	}
}