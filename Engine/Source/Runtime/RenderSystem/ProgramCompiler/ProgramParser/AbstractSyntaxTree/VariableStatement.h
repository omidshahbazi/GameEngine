// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef VARIABLE_STATEMENT_H
#define VARIABLE_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\VariableType.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API VariableStatement : public Statement, public VariableType
			{
			public:
				VariableStatement(AllocatorBase* Allocator) :
					VariableType(Allocator),
					m_InitialStatement(nullptr)
				{
				}

				virtual ~VariableStatement(void)
				{
					if (m_InitialStatement != nullptr)
						Destruct(m_InitialStatement);
				}

				Statement* GetInitialStatement(void) const
				{
					return m_InitialStatement;
				}

				void SetInitialStatement(Statement* Value)
				{
					m_InitialStatement = Value;
				}

				virtual String ToString(void) const override
				{
					String result;

					result += GetDataType()->ToString();

					result += " ";

					result += GetName();

					if (m_InitialStatement != nullptr)
					{
						result += "=";
						result += m_InitialStatement->ToString();
					}

					return result;
				}

			private:
				Statement* m_InitialStatement;
			};
		}
	}
}

#endif