// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef IF_STATEMENT_H
#define IF_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API IfStatement : public Statement, public StatementItemHolder
			{
			public:
				IfStatement(AllocatorBase* Allocator) :
					StatementItemHolder(Allocator),
					m_Condition(nullptr),
					m_Else(nullptr)
				{
				}

				virtual ~IfStatement(void)
				{
					Destruct(m_Condition);

					if (m_Else != nullptr)
						Destruct(m_Else);
				}

				Statement* GetCondition(void) const
				{
					return m_Condition;
				}

				void SetCondition(Statement* Value)
				{
					m_Condition = Value;
				}

				Statement* GetElse(void) const
				{
					return m_Else;
				}

				void SetElse(Statement* Value)
				{
					m_Else = Value;
				}

				virtual String ToString(void) const override
				{
					String result = "if (";

					result += m_Condition->ToString();

					result += ")\n{\n";

					for (auto stm : GetItems())
					{
						result += stm->ToString();
						result += ";\n";
					}

					result += "}\n";

					if (m_Else != nullptr)
						result += m_Else->ToString();

					return result;
				}

			private:
				Statement* m_Condition;
				Statement* m_Else;
			};
		}
	}
}

#endif