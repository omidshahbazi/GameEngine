// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef IF_STATEMENT_H
#define IF_STATEMENT_H

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
					class IfStatement : public Statement, public StatementItemHolder
					{
					public:
						IfStatement(AllocatorBase* Allocator) :
							StatementItemHolder(Allocator),
							m_Condition(nullptr),
							m_Else(nullptr)
						{
						}

						virtual ~IfStatement(void)
						{
							Destruct(m_Condition);

							if (m_Else != nullptr)
								Destruct(m_Else);
						}

						Statement* GetCondition(void) const
						{
							return m_Condition;
						}

						void SetCondition(Statement* Value)
						{
							m_Condition = Value;
						}

						Statement* GetElse(void) const
						{
							return m_Else;
						}

						void SetElse(Statement* Value)
						{
							m_Else = Value;
						}

					private:
						Statement* m_Condition;
						Statement* m_Else;
					};
				}
			}
		}
	}
}

#endif