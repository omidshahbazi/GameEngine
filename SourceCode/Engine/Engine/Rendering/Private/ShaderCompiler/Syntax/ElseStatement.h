// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef ELSE_STATEMENT_H
#define ELSE_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Syntax\StatementsHolder.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				namespace Syntax
				{
					class ElseStatement : public Statement, public StatementsHolder
					{
					};
				}
			}
		}
	}
}

#endif