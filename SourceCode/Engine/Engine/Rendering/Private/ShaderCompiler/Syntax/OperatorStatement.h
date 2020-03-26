// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPERATOR_STATEMENT_H
#define OPERATOR_STATEMENT_H

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
					class OperatorStatement : public Statement
					{
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