// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef UNARY_OPERATOR_STATEMENT_H
#define UNARY_OPERATOR_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>
#include <Containers\Exception.h>

namespace Engine
{
	using namespace Containers;

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

					switch (m_Operator)
					{
					case Operators::Exlamation:
					case Operators::Minus:
					case Operators::PrefixIncrement:
					case Operators::PrefixDecrement:
						result += GetOperatorSymbol(m_Operator);
						break;
					}

					result += m_Statement->ToString();

					switch (m_Operator)
					{
					case Operators::PostfixIncrement:
					case Operators::PostfixDecrement:
						result += GetOperatorSymbol(m_Operator);
						break;
					}

					return result;
				}

			public:
				static String GetOperatorSymbol(Operators Operator)
				{
					switch (Operator)
					{
					case Operators::Exlamation:
						return STRINGIZE(!);

					case Operators::Minus:
						return STRINGIZE(-);

					case Operators::PrefixIncrement:
					case Operators::PostfixIncrement:
						return STRINGIZE(++);

					case Operators::PrefixDecrement:
					case Operators::PostfixDecrement:
						return STRINGIZE(--);
					}

					THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
				}

			private:
				Operators m_Operator;
				Statement* m_Statement;
			};
		}
	}
}

#endif