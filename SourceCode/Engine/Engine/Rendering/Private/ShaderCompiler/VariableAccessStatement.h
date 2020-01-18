// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef VARIABLE_ACCESS_STATEMENT_H
#define VARIABLE_ACCESS_STATEMENT_H

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
				class VariableAccessStatement : public Statement
				{
				public:
					VariableAccessStatement(void)
					{
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