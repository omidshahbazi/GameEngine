// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef VARIABLE_ACCESS_STATEMENT_H
#define VARIABLE_ACCESS_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Syntax\Statement.h>
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
				namespace Syntax
				{
					class VariableAccessStatement : public Statement
					{
					public:
						VariableAccessStatement(void)
						{
						}

						ShaderDataType::Types EvaluateResultType(void) const override
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

						void SetVariableType(ShaderDataType::Types Type)
						{
							m_VariableType = Type;
						}

					private:
						String m_Name;
						ShaderDataType::Types m_VariableType;
					};
				}
			}
		}
	}
}

#endif