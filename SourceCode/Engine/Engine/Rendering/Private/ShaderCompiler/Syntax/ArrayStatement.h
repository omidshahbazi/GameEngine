// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef ARRAY_STATEMENT_H
#define ARRAY_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Syntax\Statement.h>
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
				namespace Syntax
				{
					class ArrayStatement : public Statement
					{
					public:
						ArrayStatement(AllocatorBase* Allocator) :
							m_Elements(Allocator)
						{
						}

						virtual ~ArrayStatement(void)
						{
						}

						const StatementItemHolder& GetELements(void) const
						{
							return m_Elements;
						}

						void AddElement(Statement* Element)
						{
							m_Elements.AddItem(Element);
						}

					private:
						StatementItemHolder m_Elements;
					};
				}
			}
		}
	}
}

#endif