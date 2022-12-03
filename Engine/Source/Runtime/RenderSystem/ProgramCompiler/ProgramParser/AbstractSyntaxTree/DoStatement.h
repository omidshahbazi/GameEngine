// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DO_STATEMENT_H
#define DO_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API DoStatement : public Statement, public StatementItemHolder
			{
			public:
				DoStatement(AllocatorBase* Allocator) :
					StatementItemHolder(Allocator),
					m_While(nullptr)
				{
				}

				~DoStatement(void)
				{
					if (m_While != nullptr)
						Destruct(m_While);
				}

				Statement* GetWhile(void) const
				{
					return m_While;
				}

				void SetWhile(Statement* Value)
				{
					m_While = Value;
				}

				virtual String ToString(void) const override
				{
					String result = "do\n{\n";

					for (auto stm : GetItems())
					{
						result += stm->ToString();
						result += ";\n";
					}

					result += "} ";

					result += m_While->ToString();

					result += ";\n";

					return result;
				}

			private:
				Statement* m_While;
			};
		}
	}
}

#endif