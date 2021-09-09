// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CASE_STATEMENT_H
#define CASE_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API CaseStatement : public Statement
			{
			public:
				CaseStatement(void)
				{
				}
			};
		}
	}
}

#endif