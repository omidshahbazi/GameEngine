// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DynamicModuleDefinition\IDynamicModule.h>
#include <GLSLCompiler\Private\GLSLCompiler.h>

namespace Engine
{
	using namespace DynamicModuleDefinition;

	namespace GLSLCompiler
	{
		namespace Private
		{
			class DynamicModule : public IDynamicModule
			{
				virtual void* Load(void) override
				{
					m_Compiler.Initialize();

					return &m_Compiler;
				}

				virtual bool Unload(void) override
				{
					m_Compiler.Uninitialize();

					return true;
				}

			private:
				GLSLCompiler m_Compiler;
			};

			DEFINE_DYNAMIC_MODULE_ENTRY_POINT(DynamicModule);
		}
	}
}