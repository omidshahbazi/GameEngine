// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef UNARY_OPERATOR_STATEMENT_H
#define UNARY_OPERATOR_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Statement.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
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

					Operators GetOperator(void) const
					{
						return m_Operator;
					}

					void SetOperator(Operators Value)
					{
						m_Operator = Value;
					}

					Statement *GetStatement(void) const
					{
						return m_Statement;
					}

					void SetStatement(Statement *Value)
					{
						m_Statement = Value;
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
					Statement *m_Statement;
				};
			}
		}
	}
}

#endif