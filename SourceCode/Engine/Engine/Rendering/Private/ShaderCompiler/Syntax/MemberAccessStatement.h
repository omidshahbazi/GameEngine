// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MEMBER_ACCESS_STATEMENT_H
#define MEMBER_ACCESS_STATEMENT_H

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
					class MemberAccessStatement : public Statement
					{
					public:
						MemberAccessStatement(void) :
							m_Left(nullptr),
							m_Right(nullptr)
						{
						}

						virtual ~MemberAccessStatement(void)
						{
							Destruct(m_Left);

							Destruct(m_Right);
						}

						Statement* GetLeft(void) const
						{
							return m_Left;
						}

						void SetLeft(Statement* Value)
						{
							m_Left = Value;
						}

						Statement* GetRight(void) const
						{
							return m_Right;
						}

						void SetRight(Statement* Value)
						{
							m_Right = Value;
						}

					private:
						Statement* m_Left;
						Statement* m_Right;
					};
				}
			}
		}
	}
}

#endif