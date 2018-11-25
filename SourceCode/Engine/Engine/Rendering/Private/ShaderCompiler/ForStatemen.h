// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef FOR_STATEMENT_H
#define FOR_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Statement.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class ForStatemen : public Statement
				{
				public:
					ForStatemen(void) :
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