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
					enum class Types
					{
						Assignment = 0,
						Variable,
						FunctionCall,
						Constant
					};

				public:
					Statement(void)
					{
					}

					virtual Types GetType(void) const = 0;
				};

				typedef Vector<Statement*> StatementList;
			}
		}
	}
}

#endif