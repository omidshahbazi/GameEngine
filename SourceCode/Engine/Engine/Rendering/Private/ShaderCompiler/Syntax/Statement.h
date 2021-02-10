// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef STATEMENT_H
#define STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Syntax\SyntaxItemHolder.h>
#include <Containers\Strings.h>

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
					class Statement
					{
					public:
						Statement(void)
						{
						}

						virtual ~Statement(void)
						{
						}

						virtual ShaderDataType::Types EvaluateResultType(void) const
						{
							return ShaderDataType::Types::Unknown;
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