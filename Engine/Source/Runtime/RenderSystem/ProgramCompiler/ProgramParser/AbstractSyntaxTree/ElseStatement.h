// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef ELSE_STATEMENT_H
#define ELSE_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API ElseStatement : public Statement, public StatementItemHolder
			{
			public:
				ElseStatement(AllocatorBase* Allocator) :
					StatementItemHolder(Allocator)
				{
				}

				virtual String ToString(void) const override
				{
					String result = "else\n{\n";

					for (auto stm : GetItems())
					{
						result += stm->ToString();
						result += ";\n";
					}

					result += "}\n";

					return result;
				}
			};
		}
	}
}

#endif