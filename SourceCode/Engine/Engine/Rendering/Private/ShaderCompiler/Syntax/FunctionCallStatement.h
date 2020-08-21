// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FUNCTION_CALL_STATEMENT_H
#define FUNCTION_CALL_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Syntax\Statement.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
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

						const String& GetFunctionName(void) const
						{
							return m_FunctionName;
						}

						void SetFunctionName(const String& FunctionName)
						{
							m_FunctionName = FunctionName;
						}

						const StatementList& GetArguments(void) const
						{
							return m_Arguments;
						}

						void AddArgument(Statement* Argument)
						{
							m_Arguments.Add(Argument);
						}

					private:
						String m_FunctionName;
						StatementList m_Arguments;
					};
				}
			}
		}
	}
}

#endif