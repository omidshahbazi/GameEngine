// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DISCARD_STATEMENT_H
#define DISCARD_STATEMENT_H

#include <Rendering\Private\ProgramCompiler\Syntax\Statement.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				namespace Syntax
				{
					class DiscardStatement : public Statement
					{
						virtual String ToString(void) const override
						{
							return "discard";
						}
					};
				}
			}
		}
	}
}

#endif