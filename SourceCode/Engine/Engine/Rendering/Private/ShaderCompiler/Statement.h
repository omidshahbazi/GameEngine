// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef STATEMENT_H
#define STATEMENT_H

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

#endif