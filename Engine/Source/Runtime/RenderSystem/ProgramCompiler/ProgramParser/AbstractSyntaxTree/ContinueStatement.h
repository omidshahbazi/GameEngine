// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONTINUER_STATEMENT_H
#define CONTINUER_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API ContinueStatement : public Statement
			{
			public:
				ContinueStatement(void)
				{
				}

				virtual String ToString(void) const override
				{
					return "continue;";
				}
			};
		}
	}
}

#endif