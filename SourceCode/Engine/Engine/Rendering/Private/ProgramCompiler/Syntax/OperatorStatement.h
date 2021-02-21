// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPERATOR_STATEMENT_H
#define OPERATOR_STATEMENT_H

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
					class OperatorStatement : public Statement
					{
					private:
					public:
						enum class Operators
						{
							Multipication = 0,
							Division,
							Remainder,
							Addition,
							Subtraction,

							Assignment,
							AdditionAssignment,
							SubtractionAssignment,
							MultipicationAssignment,
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

						ProgramDataTypes EvaluateResultType(void) const override
						{
							static ProgramDataTypes MULTIPLY_RESULT[(uint8)ProgramDataTypes::Unknown][(uint8)ProgramDataTypes::Unknown] =
							{
								{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
								{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
								{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float,ProgramDataTypes::Double,ProgramDataTypes::Float2,ProgramDataTypes::Double2,ProgramDataTypes::Float3,ProgramDataTypes::Double3,ProgramDataTypes::Float4,ProgramDataTypes::Double4,ProgramDataTypes::Matrix4 },
								{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double,ProgramDataTypes::Double,ProgramDataTypes::Double2,ProgramDataTypes::Double2,ProgramDataTypes::Double3,ProgramDataTypes::Double3,ProgramDataTypes::Double4,ProgramDataTypes::Double4,ProgramDataTypes::Matrix4 },
								{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float2,ProgramDataTypes::Double2,ProgramDataTypes::Float2,ProgramDataTypes::Double2,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
								{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double2,ProgramDataTypes::Double2,ProgramDataTypes::Double2,ProgramDataTypes::Double2,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
								{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float3,ProgramDataTypes::Double3,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float3,ProgramDataTypes::Double3,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
								{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double3,ProgramDataTypes::Double3,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double3,ProgramDataTypes::Double3,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
								{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float4,ProgramDataTypes::Double4,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float4,ProgramDataTypes::Double4,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
								{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double4,ProgramDataTypes::Double4,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double4,ProgramDataTypes::Double4,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
								{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Matrix4,ProgramDataTypes::Matrix4,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float4,ProgramDataTypes::Double4,ProgramDataTypes::Matrix4 },
							};

							ProgramDataTypes leftType = m_Left->EvaluateResultType();
							ProgramDataTypes rightType = m_Left->EvaluateResultType();

							if (m_Operator == Operators::Multipication || m_Operator == Operators::Division)
								return MULTIPLY_RESULT[(uint8)leftType][(uint8)rightType];

							if (m_Operator == Operators::Remainder ||
								m_Operator == Operators::Addition ||
								m_Operator == Operators::Subtraction)
								return leftType;

							if (m_Operator == Operators::EqualCheck ||
								m_Operator == Operators::NotEqualCheck ||
								m_Operator == Operators::LessCheck ||
								m_Operator == Operators::LessEqualCheck ||
								m_Operator == Operators::GreaterCheck ||
								m_Operator == Operators::GreaterEqualCheck ||
								m_Operator == Operators::LogicalAnd ||
								m_Operator == Operators::LogicalOr)
								return ProgramDataTypes::Bool;

							return ProgramDataTypes::Unknown;
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
							case Operators::Multipication:
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
							case Operators::MultipicationAssignment:
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
	}
}

#endif