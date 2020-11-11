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

						ShaderDataType::Types EvaluateResultType(void) const override
						{
							ShaderDataType::Types leftType = m_Left->EvaluateResultType();

							if (leftType == ShaderDataType::Types::Float2 ||
								leftType == ShaderDataType::Types::Float3 ||
								leftType == ShaderDataType::Types::Float4)
								return ShaderDataType::Types::Float;

							if (leftType == ShaderDataType::Types::Double2 ||
								leftType == ShaderDataType::Types::Double3 ||
								leftType == ShaderDataType::Types::Double4)
								return ShaderDataType::Types::Double;

							if (leftType == ShaderDataType::Types::Matrix4)
								return ShaderDataType::Types::Float;

							return ShaderDataType::Types::Unknown;
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