// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef UNARY_OPERATOR_STATEMENT_H
#define UNARY_OPERATOR_STATEMENT_H

#include <Rendering\Private\ProgramCompiler\Syntax\Statement.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				namespace Syntax
				{
					class UnaryOperatorStatement : public Statement
					{
					public:
						enum class Operators
						{
							Minus = 0,
							Unknown
						};

					public:
						UnaryOperatorStatement(void) :
							m_Statement(nullptr)
						{
						}

						virtual ~UnaryOperatorStatement(void)
						{
							Destruct(m_Statement);
						}

						ProgramDataTypes EvaluateResultType(void) const override
						{
							return m_Statement->EvaluateResultType();
						}

						Operators GetOperator(void) const
						{
							return m_Operator;
						}

						void SetOperator(Operators Value)
						{
							m_Operator = Value;
						}

						Statement* GetStatement(void) const
						{
							return m_Statement;
						}

						void SetStatement(Statement* Value)
						{
							m_Statement = Value;
						}

						virtual String ToString(void) const override
						{
							String result;

							result += GetOperatorSymbol(m_Operator);

							result += m_Statement->ToString();

							return result;
						}

					public:
						static String GetOperatorSymbol(Operators Operator)
						{
							switch (Operator)
							{
							case Operators::Minus:
								return "-";
							}

							return "";
						}

					private:
						Operators m_Operator;
						Statement* m_Statement;
					};
				}
			}
		}
	}
}

#endif