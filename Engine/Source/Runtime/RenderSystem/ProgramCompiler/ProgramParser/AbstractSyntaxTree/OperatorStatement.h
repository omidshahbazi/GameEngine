// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPERATOR_STATEMENT_H
#define OPERATOR_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>
#include <Containers\Exception.h>

namespace Engine
{
	using namespace Containers;

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
						return STRINGIZE(*);
					case Operators::Division:
						return STRINGIZE(/);
					case Operators::Remainder:
						return STRINGIZE(%);
					case Operators::Addition:
						return STRINGIZE(+);
					case Operators::Subtraction:
						return STRINGIZE(-);
					case Operators::Assignment:
						return STRINGIZE(=);
					case Operators::AdditionAssignment:
						return STRINGIZE(+=);
					case Operators::SubtractionAssignment:
						return STRINGIZE(-=);
					case Operators::MultiplicationAssignment:
						return STRINGIZE(*=);
					case Operators::DivisionAssignment:
						return STRINGIZE(/=);
					case Operators::EqualCheck:
						return STRINGIZE(==);
					case Operators::NotEqualCheck:
						return STRINGIZE(!=);
					case Operators::LessCheck:
						return STRINGIZE(<);
					case Operators::LessEqualCheck:
						return STRINGIZE(<=);
					case Operators::GreaterCheck:
						return STRINGIZE(>);
					case Operators::GreaterEqualCheck:
						return STRINGIZE(>=);
					case Operators::LogicalAnd:
						return STRINGIZE(&&);
					case Operators::LogicalOr:
						return STRINGIZE(||);
					}

					THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
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