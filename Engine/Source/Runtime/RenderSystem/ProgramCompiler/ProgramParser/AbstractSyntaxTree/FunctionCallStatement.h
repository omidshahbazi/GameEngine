// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FUNCTION_CALL_STATEMENT_H
#define FUNCTION_CALL_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API FunctionCallStatement : public Statement
			{
			public:
				FunctionCallStatement(AllocatorBase* Allocator) :
					m_Arguments(Allocator),
					m_FunctionName(Allocator)
				{
				}

				virtual ~FunctionCallStatement(void)
				{
				}

				const String& GetFunctionName(void) const
				{
					return m_FunctionName;
				}

				void SetFunctionName(const String& FunctionName)
				{
					m_FunctionName = FunctionName;
				}

				void AddArgument(Statement* Statement)
				{
					m_Arguments.AddItem(Statement);
				}

				const StatementItemHolder* GetArguments(void) const
				{
					return &m_Arguments;
				}

				virtual String ToString(void) const override
				{
					String result = m_FunctionName;
					result += "(";

					bool isFirst = true;
					for (auto stm : m_Arguments.GetItems())
					{
						if (!isFirst)
							result += ", ";
						isFirst = false;

						result += stm->ToString();
					}

					result += ")";

					return result;
				}

			private:
				String m_FunctionName;
				StatementItemHolder m_Arguments;
			};
		}
	}
}

#endif