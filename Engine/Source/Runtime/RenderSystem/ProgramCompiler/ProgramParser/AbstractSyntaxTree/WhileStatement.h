// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef WHILE_STATEMENT_H
#define WHILE_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API WhileStatement : public Statement
			{
			public:
				WhileStatement(void)
				{
				}
			};
		}
	}
}

#endif