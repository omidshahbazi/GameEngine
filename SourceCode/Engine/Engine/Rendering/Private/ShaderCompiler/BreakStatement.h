// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BREAK_STATEMENT_H
#define BREAK_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Statement.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class BreakStatement : public Statement
				{
				public:
					BreakStatement(void) :
						m_Left(nullptr)
					{
					}

					Statement *GetLeft(void) const
					{
						return m_Left;
					}

					void SetLeft(Statement *Value)
					{
						m_Left = Value;
					}

				private:
					Statement * m_Left;
				};
			}
		}
	}
}

#endif