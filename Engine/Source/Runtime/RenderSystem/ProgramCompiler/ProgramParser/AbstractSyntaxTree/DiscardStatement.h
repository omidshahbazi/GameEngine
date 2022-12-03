// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DISCARD_STATEMENT_H
#define DISCARD_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API DiscardStatement : public Statement
			{
			public:
				virtual String ToString(void) const override
				{
					return "discard;\n";
				}
			};
		}
	}
}

#endif