// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FOR_STATEMENT_H
#define FOR_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API ForStatement : public Statement
			{
			public:
				ForStatement(void)
				{
				}
			};
		}
	}
}

#endif