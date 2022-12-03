// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DEFAULT_STATEMENT_H
#define DEFAULT_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API DefaultStatement : public Statement, public StatementItemHolder
			{
			public:
				DefaultStatement(AllocatorBase* Allocator) :
					StatementItemHolder(Allocator)
				{
				}

				virtual String ToString(void) const override
				{
					String result = "default:\n";

					if (GetItems().GetSize() != 0)
					{
						result += "{\n";
						for (auto stm : GetItems())
						{
							result += stm->ToString();
							result += ";\n";
						}

						result += "}";
					}

					return result;
				}
			};
		}
	}
}

#endif