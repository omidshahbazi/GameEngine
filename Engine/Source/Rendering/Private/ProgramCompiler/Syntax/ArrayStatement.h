// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef ARRAY_STATEMENT_H
#define ARRAY_STATEMENT_H

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
					class ArrayStatement : public Statement, public StatementItemHolder
					{
					public:
						ArrayStatement(AllocatorBase* Allocator) :
							StatementItemHolder(Allocator)
						{
						}

						virtual ~ArrayStatement(void)
						{
						}
					};
				}
			}
		}
	}
}

#endif