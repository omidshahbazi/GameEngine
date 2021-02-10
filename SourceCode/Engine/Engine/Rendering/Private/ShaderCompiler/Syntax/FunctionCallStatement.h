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

						virtual ~FunctionCallStatement(void)
						{
						}

						ShaderDataType::Types EvaluateResultType(void) const override
						{
							return ShaderDataType::Types::Unknown;
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