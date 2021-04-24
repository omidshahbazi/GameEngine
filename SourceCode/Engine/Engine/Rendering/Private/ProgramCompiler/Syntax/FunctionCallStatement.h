// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FUNCTION_CALL_STATEMENT_H
#define FUNCTION_CALL_STATEMENT_H

#include <Rendering\Private\ProgramCompiler\Syntax\Statement.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				namespace Syntax
				{
					class FunctionCallStatement : public Statement
					{
					public:
						FunctionCallStatement(AllocatorBase* Allocator) :
							m_Arguments(Allocator)
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

						const StatementItemHolder& GetArguments(void) const
						{
							return m_Arguments;
						}

						void AddArgument(Statement* Argument)
						{
							m_Arguments.AddItem(Argument);
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
	}
}

#endif