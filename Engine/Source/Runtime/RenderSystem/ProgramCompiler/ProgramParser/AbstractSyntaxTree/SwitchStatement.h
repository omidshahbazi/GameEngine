// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SWITCH_STATEMENT_H
#define SWITCH_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API SwitchStatement : public Statement, public StatementItemHolder
			{
			public:
				SwitchStatement(AllocatorBase* Allocator) :
					StatementItemHolder(Allocator),
					m_Selector(nullptr)
				{
				}

				virtual ~SwitchStatement(void)
				{
					Destruct(m_Selector);
				}

				Statement* GetSelector(void) const
				{
					return m_Selector;
				}

				void SetSelector(Statement* Value)
				{
					m_Selector = Value;
				}

				virtual String ToString(void) const override
				{
					String result = "switch(";

					result += m_Selector->ToString();

					result += ")\n{\n";

					for (auto stm : GetItems())
					{
						result += stm->ToString();
						result += "\n";
					}

					result += "}\n";

					return result;
				}

			private:
				Statement* m_Selector;
			};
		}
	}
}

#endif