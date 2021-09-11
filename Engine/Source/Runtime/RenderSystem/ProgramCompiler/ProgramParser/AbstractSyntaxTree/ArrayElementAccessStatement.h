// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef ARRAY_ELEMENT_ACCESS_STATEMENT_H
#define ARRAY_ELEMENT_ACCESS_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API ArrayElementAccessStatement : public Statement
			{
			public:
				ArrayElementAccessStatement(void) :
					m_ArrayStatement(nullptr),
					m_ElementStatement(nullptr)
				{
				}

				virtual ~ArrayElementAccessStatement(void)
				{
					Destruct(m_ArrayStatement);
					Destruct(m_ElementStatement);
				}

				Statement* GetArrayStatement(void) const
				{
					return m_ArrayStatement;
				}

				void SetArrayStatement(Statement* Value)
				{
					m_ArrayStatement = Value;
				}

				Statement* GetElementStatement(void) const
				{
					return m_ElementStatement;
				}

				void SetElementStatement(Statement* Value)
				{
					m_ElementStatement = Value;
				}

				virtual String ToString(void) const override
				{
					String result;

					result += m_ArrayStatement->ToString();

					result += "[";

					result += m_ElementStatement->ToString();

					result += "]";

					return result;
				}

			private:
				Statement* m_ArrayStatement;
				Statement* m_ElementStatement;
			};
		}
	}
}

#endif