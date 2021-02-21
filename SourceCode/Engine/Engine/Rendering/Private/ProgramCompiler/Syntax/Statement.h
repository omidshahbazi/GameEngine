// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef STATEMENT_H
#define STATEMENT_H

#include <Rendering\Private\ProgramCompiler\Syntax\SyntaxItemHolder.h>
#include <Rendering\ProgramDataTypes.h>
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
					class Statement
					{
					public:
						Statement(void)
						{
						}

						virtual ~Statement(void)
						{
						}

						virtual ProgramDataTypes EvaluateResultType(void) const
						{
							return ProgramDataTypes::Unknown;
						}

						virtual String ToString(void) const
						{
							return "";
						}
					};

					//typedef Vector<Statement*> StatementList;
					typedef SyntaxItemHolder<Statement> StatementItemHolder;
				}
			}
		}
	}
}

#endif