// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef VARIABLE_ACCESS_STATEMENT_H
#define VARIABLE_ACCESS_STATEMENT_H

#include <Rendering\Private\ProgramCompiler\Syntax\Statement.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				namespace Syntax
				{
					class VariableAccessStatement : public Statement
					{
					public:
						VariableAccessStatement(void)
						{
						}

						ProgramDataTypes EvaluateResultType(void) const override
						{
							return m_VariableType;
						}

						const String& GetName(void) const
						{
							return m_Name;
						}

						void SetName(const String& Name)
						{
							m_Name = Name;
						}

						void SetVariableType(ProgramDataTypes Type)
						{
							m_VariableType = Type;
						}

						virtual String ToString(void) const override
						{
							return m_Name;
						}

					private:
						String m_Name;
						ProgramDataTypes m_VariableType;
					};
				}
			}
		}
	}
}

#endif