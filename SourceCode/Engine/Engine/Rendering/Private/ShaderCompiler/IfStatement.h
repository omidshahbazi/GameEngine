// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef IF_STATEMENT_H
#define IF_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Statement.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class IfStatement : public Statement
				{
				public:
					IfStatement(void) :
						m_Condition(nullptr),
						m_Else(nullptr)
					{
					}

					Statement *GetCondition(void) const
					{
						return m_Condition;
					}

					void SetCondition(Statement *Value)
					{
						m_Condition = Value;
					}

					const StatementList &GetStatements(void) const
					{
						return m_Statements;
					}

					void AddStatement(Statement *Statement)
					{
						m_Statements.Add(Statement);
					}

					Statement *GetElse(void) const
					{
						return m_Else;
					}

					void SetElse(Statement *Value)
					{
						m_Else = Value;
					}

				private:
					Statement * m_Condition;
					StatementList m_Statements;
					Statement * m_Else;
				};
			}
		}
	}
}

#endif