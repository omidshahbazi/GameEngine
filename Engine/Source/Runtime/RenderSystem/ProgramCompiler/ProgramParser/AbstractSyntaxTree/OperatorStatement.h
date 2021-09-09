// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPERATOR_STATEMENT_H
#define OPERATOR_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>

namespace Engine
{
	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API OperatorStatement : public Statement
			{
			private:
			public:
				enum class Operators
				{
					Multiplication = 0,
					Division,
					Remainder,
					Addition,
					Subtraction,

					Assignment,
					AdditionAssignment,
					SubtractionAssignment,
					MultiplicationAssignment,
					DivisionAssignment,

					EqualCheck,
					NotEqualCheck,
					LessCheck,
					LessEqualCheck,
					GreaterCheck,
					GreaterEqualCheck,

					LogicalAnd,
					LogicalOr,

					Unknown
				};

			public:
				OperatorStatement(void) :
					m_Left(nullptr),
					m_Right(nullptr)
				{
				}

				virtual ~OperatorStatement(void)
				{
					Destruct(m_Left);

					Destruct(m_Right);
				}

				Operators GetOperator(void) const
				{
					return m_Operator;
				}

				void SetOperator(Operators Value)
				{
					m_Operator = Value;
				}

				Statement* GetLeft(void) const
				{
					return m_Left;
				}

				void SetLeft(Statement* Value)
				{
					m_Left = Value;
				}

				Statement* GetRight(void) const
				{
					return m_Right;
				}

				void SetRight(Statement* Value)
				{
					m_Right = Value;
				}

				virtual String ToString(void) const override
				{
					String result;

					result += m_Left->ToString();

					result += GetOperatorSymbol(m_Operator);

					result += m_Right->ToString();

					return result;
				}

			public:
				static String GetOperatorSymbol(Operators Operator)
				{
					switch (Operator)
					{
					case Operators::Multiplication:
						return "*";
					case Operators::Division:
						return "/";
					case Operators::Remainder:
						return "%";
					case Operators::Addition:
						return "+";
					case Operators::Subtraction:
						return "-";
					case Operators::Assignment:
						return "=";
					case Operators::AdditionAssignment:
						return "+=";
					case Operators::SubtractionAssignment:
						return "+=";
					case Operators::MultiplicationAssignment:
						return "*=";
					case Operators::DivisionAssignment:
						return "/=";
					case Operators::EqualCheck:
						return "==";
					case Operators::NotEqualCheck:
						return "!=";
					case Operators::LessCheck:
						return "<";
					case Operators::LessEqualCheck:
						return "<=";
					case Operators::GreaterCheck:
						return ">";
					case Operators::GreaterEqualCheck:
						return ">=";
					case Operators::LogicalAnd:
						return "&&";
					case Operators::LogicalOr:
						return "||";
					}

					return "";
				}

			private:
				Operators m_Operator;
				Statement* m_Left;
				Statement* m_Right;
			};
		}
	}
}
#endif