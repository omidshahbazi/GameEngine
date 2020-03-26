// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef IF_STATEMENT_H
#define IF_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Syntax\StatementsHolder.h>

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
					class IfStatement : public Statement, public StatementsHolder
					{
					public:
						IfStatement(void) :
							m_Condition(nullptr),
							m_Else(nullptr)
						{
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