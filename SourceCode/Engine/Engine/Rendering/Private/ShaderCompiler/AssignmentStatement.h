// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef ASSIGNMENT_STATEMENT_H
#define ASSIGNMENT_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Statement.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class AssignmentStatement : public Statement
				{
				public:
					AssignmentStatement(void) :
						m_Left(nullptr),
						m_Right(nullptr)
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

					Statement *GetRight(void) const
					{
						return m_Right;
					}

					void SetRight(Statement *Value)
					{
						m_Right = Value;
					}

				private:
					Statement * m_Left;
					Statement *m_Right;
				};
			}
		}
	}
}

#endif