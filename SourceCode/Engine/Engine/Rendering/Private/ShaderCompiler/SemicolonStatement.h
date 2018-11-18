// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SEMICOLON_STATEMENT_H
#define SEMICOLON_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Statement.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class SemicolonStatement : public Statement
				{
				public:
					SemicolonStatement(void)
					{
					}
				};
			}
		}
	}
}

#endif