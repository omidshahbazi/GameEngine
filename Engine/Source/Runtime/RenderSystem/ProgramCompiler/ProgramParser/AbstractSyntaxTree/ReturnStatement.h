// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RETURN_STATEMENT_H
#define RETURN_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API ReturnStatement : public Statement
			{
			public:
				ReturnStatement(void) :
					m_Statement(nullptr)
				{
				}

				virtual ~ReturnStatement(void)
				{
					if (m_Statement != nullptr)
						Destruct(m_Statement);
				}

				Statement* GetStatement(void) const
				{
					return m_Statement;
				}

				void SetStatement(Statement* Value)
				{
					m_Statement = Value;
				}

				virtual String ToString(void) const override
				{
					String result;

					result += "return ";

					if (m_Statement != nullptr)
						result += m_Statement->ToString();

					result += ";\n";

					return result;
				}

			private:
				Statement* m_Statement;
			};
		}
	}
}

#endif