// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SWITCH_STATEMENT_H
#define SWITCH_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API SwitchStatement : public Statement
			{
			public:
				SwitchStatement(void)
				{
				}
			};
		}
	}
}

#endif