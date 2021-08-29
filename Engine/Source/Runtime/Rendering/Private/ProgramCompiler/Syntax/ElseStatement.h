// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef ELSE_STATEMENT_H
#define ELSE_STATEMENT_H

#include <Rendering\Private\ProgramCompiler\Syntax\Statement.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				namespace Syntax
				{
					class ElseStatement : public Statement, public StatementItemHolder
					{
					public:
						ElseStatement(AllocatorBase* Allocator) :
							StatementItemHolder(Allocator)
						{
						}

						virtual String ToString(void) const override
						{
							String result = "else\n{";

							for (auto stm : GetItems())
							{
								result += stm->ToString();
								result += "\n";
							}

							result += "}";

							return result;
						}
					};
				}
			}
		}
	}
}

#endif