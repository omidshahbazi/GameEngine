// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef VARIABLE_STATEMENT_H
#define VARIABLE_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Statement.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class VariableStatement : public Statement
				{
				public:
					VariableStatement(void)
					{
					}

					Types GetType(void) const override
					{
						return Types::Variable;
					}

					const String &GetName(void) const
					{
						return m_Name;
					}

					void SetName(const String &Name)
					{
						m_Name = Name;
					}

				private:
					String m_Name;
				};
			}
		}
	}
}

#endif