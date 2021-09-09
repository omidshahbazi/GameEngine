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
			class PROGRAMPARSER_API DoStatement : public Statement
			{
			public:
				DoStatement(void)
				{
				}
			};
		}
	}
}

#endif