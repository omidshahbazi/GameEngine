// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef STATEMENT_H
#define STATEMENT_H

#include <Containers\Strings.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				namespace Syntax
				{
					class Statement
					{
					public:
						Statement(void)
						{
						}

						virtual String ToString(void) const
						{
							return "";
						}
					};

					typedef Vector<Statement*> StatementList;
				}
			}
		}
	}
}

#endif