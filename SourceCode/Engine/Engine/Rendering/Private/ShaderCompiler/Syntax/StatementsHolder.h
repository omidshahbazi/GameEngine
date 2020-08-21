// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef STATEMENTS_HOLDER_H
#define STATEMENTS_HOLDER_H

#include <Rendering\Private\ShaderCompiler\Syntax\Statement.h>

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
					class StatementsHolder
					{
					public:
						StatementsHolder(AllocatorBase* Allocator) :
							m_Statements(Allocator)
						{
						}

						virtual ~StatementsHolder(void)
						{
						}

						const StatementList& GetStatements(void) const
						{
							return m_Statements;
						}

						void AddStatement(Statement* Statement)
						{
							m_Statements.Add(Statement);
						}

					private:
						StatementList m_Statements;
					};
				}
			}
		}
	}
}

#endif