// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef UNARY_OPERATOR_STATEMENT_H
#define UNARY_OPERATOR_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API UnaryOperatorStatement : public Statement
			{
			public:
				enum class Operators
				{
					Exlamation = 0,
					Minus,
					PrefixIncrement,
					PrefixDecrement,
					PostfixIncrement,
					PostfixDecrement,
					Unknown
				};

			public:
				UnaryOperatorStatement(void) :
					m_Operator(Operators::Unknown),
					m_Statement(nullptr)
				{
				}

				virtual ~UnaryOperatorStatement(void)
				{
					Destruct(m_Statement);
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

					result += GetPrefixOperatorSymbol(m_Operator);

					result += m_Statement->ToString();

					result += GetPostfixOperatorSymbol(m_Operator);

					return result;
				}

			public:
				static String GetPrefixOperatorSymbol(Operators Operator)
				{
					switch (Operator)
					{
					case Operators::Exlamation:
						return "!";
					case Operators::Minus:
						return "-";
					case Operators::PrefixIncrement:
						return "++";
					case Operators::PrefixDecrement:
						return "--";
					}

					return "";
				}

				static String GetPostfixOperatorSymbol(Operators Operator)
				{
					switch (Operator)
					{
					case Operators::PostfixIncrement:
						return "++";
					case Operators::PostfixDecrement:
						return "--";
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

#endif