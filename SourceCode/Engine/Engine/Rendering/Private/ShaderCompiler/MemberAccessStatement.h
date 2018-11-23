// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef MEMBER_ACCESS_STATEMENT_H
#define MEMBER_ACCESS_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Statement.h>
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
				class MemberAccessStatement : public Statement
				{
				public:
					MemberAccessStatement(void) :
						m_Member(nullptr)
					{
					}

					const String &GetName(void) const
					{
						return m_Name;
					}

					void SetName(const String &Name)
					{
						m_Name = Name;
					}

					Statement *GetMember(void) const
					{
						return m_Member;
					}

					void SetMember(Statement *Value)
					{
						m_Member = Value;
					}

				private:
					String m_Name;
					Statement *m_Member;
				};
			}
		}
	}
}

#endif