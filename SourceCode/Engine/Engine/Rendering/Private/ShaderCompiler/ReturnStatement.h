// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RETURN_STATEMENT_H
#define RETURN_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Statement.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class ReturnStatement : public Statement
				{
				public:
					ReturnStatement(void) :
						m_Statement(nullptr)
					{
					}

					Statement *GetStatement(void) const
					{
						return m_Statement;
					}

					void SetStatement(Statement *Value)
					{
						m_Statement = Value;
					}

				private:
					Statement *m_Statement;
				};
			}
		}
	}
}

#endif