// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef FUNCTION_CALL_STATEMENT_H
#define FUNCTION_CALL_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Statement.h>
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
				class FunctionCallStatement : public Statement
				{
				public:
					FunctionCallStatement(void)
					{
					}

					const String &GetFunctionName(void) const
					{
						return m_FunctionName;
					}

					void SetFunctionName(const String &FunctionName)
					{
						m_FunctionName = FunctionName;
					}

					const StatementList &GetArguments(void) const
					{
						return m_Arguments;
					}

					void InsertArgumentStatement(uint32 Index, Statement *ArgumentStatement)
					{
						m_Arguments.Insert(Index, ArgumentStatement);
					}

					void AddArgumentStatement(Statement *ArgumentStatement)
					{
						m_Arguments.Add(ArgumentStatement);
					}

				private:
					String m_FunctionName;
					StatementList m_Arguments;
				};
			}
		}
	}
}

#endif