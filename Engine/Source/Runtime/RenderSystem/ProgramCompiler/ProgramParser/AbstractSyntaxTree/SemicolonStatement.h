// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SEMICOLON_STATEMENT_H
#define SEMICOLON_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API SemicolonStatement : public Statement
			{
			public:
				SemicolonStatement(void)
				{
				}

				virtual String ToString(void) const override
				{
					return ";";
				}
			};
		}
	}
}

#endif