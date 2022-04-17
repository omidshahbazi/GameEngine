// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FOR_STATEMENT_H
#define FOR_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API ForStatement : public Statement, public StatementItemHolder
			{
			public:
				ForStatement(AllocatorBase* Allocator) :
					StatementItemHolder(Allocator),
					m_Initializer(nullptr),
					m_Condition(nullptr),
					m_Step(nullptr)
				{
				}

				~ForStatement(void)
				{
					if (m_Initializer != nullptr)
						Destruct(m_Initializer);

					if (m_Condition != nullptr)
						Destruct(m_Condition);

					if (m_Step != nullptr)
						Destruct(m_Step);
				}

				Statement* GetInitializer(void) const
				{
					return m_Initializer;
				}

				void SetInitializer(Statement* Value)
				{
					m_Initializer = Value;
				}

				Statement* GetCondition(void) const
				{
					return m_Condition;
				}

				void SetCondition(Statement* Value)
				{
					m_Condition = Value;
				}

				Statement* GetStep(void) const
				{
					return m_Step;
				}

				void SetStep(Statement* Value)
				{
					m_Step = Value;
				}

				virtual String ToString(void) const override
				{
					String result = "for (";

					if (m_Initializer != nullptr)
						result += m_Initializer->ToString();

					result += ";";

					if (m_Condition != nullptr)
						result += m_Condition->ToString();

					result += ";";

					if (m_Step != nullptr)
						result += m_Step->ToString();

					result += ")\n";

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
				Statement* m_Initializer;
				Statement* m_Condition;
				Statement* m_Step;
			};
		}
	}
}

#endif