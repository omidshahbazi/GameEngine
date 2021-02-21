// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RETURN_STATEMENT_H
#define RETURN_STATEMENT_H

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

						virtual String ToString(void) const override
						{
							String result;

							result += "return ";

							if (m_Statement != nullptr)
								result += m_Statement->ToString();

							return result;
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