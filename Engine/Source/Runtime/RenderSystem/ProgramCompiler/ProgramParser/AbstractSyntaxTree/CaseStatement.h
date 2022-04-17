// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CASE_STATEMENT_H
#define CASE_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API CaseStatement : public Statement, public StatementItemHolder
			{
			public:
				CaseStatement(AllocatorBase* Allocator) :
					StatementItemHolder(Allocator),
					m_Condition(nullptr)
				{
				}

				virtual ~CaseStatement(void)
				{
					Destruct(m_Condition);
				}

				Statement* GetCondition(void) const
				{
					return m_Condition;
				}

				void SetCondition(Statement* Value)
				{
					m_Condition = Value;
				}

				virtual String ToString(void) const override
				{
					String result = "case ";

					result += m_Condition->ToString();

					result += ":\n";

					if (GetItems().GetSize() != 0)
					{
						result += "{";

						for (auto stm : GetItems())
						{
							result += stm->ToString();
							result += "\n";
						}

						result += "}";
					}

					return result;
				}

			private:
				Statement* m_Condition;
			};
		}
	}
}

#endif