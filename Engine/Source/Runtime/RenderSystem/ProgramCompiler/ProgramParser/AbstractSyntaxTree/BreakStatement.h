// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BREAK_STATEMENT_H
#define BREAK_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API BreakStatement : public Statement
			{
			public:
				BreakStatement(void)
				{
				}

				virtual String ToString(void) const override
				{
					return "break";
				}
			};
		}
	}
}

#endif