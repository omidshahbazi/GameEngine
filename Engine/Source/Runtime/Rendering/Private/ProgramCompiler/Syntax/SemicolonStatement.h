// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SEMICOLON_STATEMENT_H
#define SEMICOLON_STATEMENT_H

#include <Rendering\Private\ProgramCompiler\Syntax\Statement.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				namespace Syntax
				{
					class SemicolonStatement : public Statement
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
	}
}

#endif