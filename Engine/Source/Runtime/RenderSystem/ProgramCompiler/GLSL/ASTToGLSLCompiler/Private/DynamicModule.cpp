// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DynamicModuleDefinition\IDynamicModule.h>
#include <ASTToGLSLCompiler\Private\ASTToGLSLCompiler.h>

namespace Engine
{
	using namespace DynamicModuleDefinition;

	namespace ASTToGLSLCompiler
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
				ASTToGLSLCompiler m_Compiler;
			};

			DEFINE_DYNAMIC_MODULE_ENTRY_POINT(DynamicModule);
		}
	}
}