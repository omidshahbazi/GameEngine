// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RETURN_STATEMENT_H
#define RETURN_STATEMENT_H

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
					class ReturnStatement : public Statement
					{
					public:
						ReturnStatement(void) :
							m_Statement(nullptr)
						{
						}

						virtual ~ReturnStatement(void)
						{
							if (m_Statement != nullptr)
								Destruct(m_Statement);
						}

						Statement* GetStatement(void) const
						{
							return m_Statement;
						}

						void SetStatement(Statement* Value)
						{
							m_Statement = Value;
						}

					private:
						Statement* m_Statement;
					};
				}
			}
		}
	}
}

#endif