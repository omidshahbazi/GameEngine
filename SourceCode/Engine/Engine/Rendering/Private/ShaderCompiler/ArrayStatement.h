// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef ARRAY_STATEMENT_H
#define ARRAY_STATEMENT_H

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
				class ArrayStatement : public Statement
				{
				public:
					ArrayStatement(void)
					{
					}

					const StatementList &GetELements(void) const
					{
						return m_Elements;
					}

					void AddElement(Statement *Element)
					{
						m_Elements.Add(Element);
					}

				private:
					StatementList m_Elements;
				};
			}
		}
	}
}

#endif