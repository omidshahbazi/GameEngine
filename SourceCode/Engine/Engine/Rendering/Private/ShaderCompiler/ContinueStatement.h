// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONTINUER_STATEMENT_H
#define CONTINUER_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Statement.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class ContinueStatement : public Statement
				{
				public:
					ContinueStatement(void) :
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