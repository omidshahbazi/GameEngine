// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef STATEMENT_H
#define STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\SyntaxItemHolder.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API Statement
			{
			public:
				Statement(void)
				{
				}

				virtual ~Statement(void)
				{
				}

				virtual String ToString(void) const
				{
					return "";
				}
			};

			typedef SyntaxItemHolder<Statement> StatementItemHolder;
		}
	}
}

#endif