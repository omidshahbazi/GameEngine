// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef ARRAY_ELEMENT_ACCESS_STATEMENT_H
#define ARRAY_ELEMENT_ACCESS_STATEMENT_H

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
					class ArrayElementAccessStatement : public Statement
					{
					public:
						ArrayElementAccessStatement(void) :
							m_ArrayStatement(nullptr),
							m_ElementStatement(nullptr)
						{
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

					private:
						Statement* m_ArrayStatement;
						Statement* m_ElementStatement;
					};
				}
			}
		}
	}
}

#endif