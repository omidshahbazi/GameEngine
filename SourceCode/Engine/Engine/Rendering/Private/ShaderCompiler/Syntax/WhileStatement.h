// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef WHILE_STATEMENT_H
#define WHILE_STATEMENT_H

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
					class WhileStatement : public Statement
					{
					public:
						WhileStatement(void) :
							m_Left(nullptr)
						{
						}

						Statement* GetLeft(void) const
						{
							return m_Left;
						}

						void SetLeft(Statement* Value)
						{
							m_Left = Value;
						}

					private:
						Statement* m_Left;
					};
				}
			}
		}
	}
}

#endif