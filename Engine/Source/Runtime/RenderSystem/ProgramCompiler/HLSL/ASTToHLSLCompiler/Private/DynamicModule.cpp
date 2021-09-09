// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DynamicModuleDefinition\IDynamicModule.h>
#include <ASTToHLSLCompiler\Private\ASTToHLSLCompiler.h>

namespace Engine
{
	using namespace DynamicModuleDefinition;

	namespace ASTToHLSLCompiler
	{
		namespace Private
		{
			class DynamicModule : public IDynamicModule
			{
				virtual void* Load(void) override
				{
					return &m_Compiler;
				}

				virtual bool Unload(void) override
				{
					return true;
				}

			private:
				ASTToHLSLCompiler m_Compiler;
			};

			DEFINE_DYNAMIC_MODULE_ENTRY_POINT(DynamicModule);
		}
	}
}