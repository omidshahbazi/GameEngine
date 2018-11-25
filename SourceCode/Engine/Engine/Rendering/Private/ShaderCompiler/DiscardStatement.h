// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef DISCARD_STATEMENT_H
#define DISCARD_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Statement.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class DiscardStatement : public Statement
				{
				public:
					DiscardStatement(void) :
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