// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONSTANT_STATEMENT_H
#define CONSTANT_STATEMENT_H

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
					class ConstantStatement : public Statement
					{
					public:
						enum class Types
						{
							Boolean = 0,
							Float
						};

					public:
						ConstantStatement(void)
						{
						}

						Types GetType(void) const
						{
							return m_Type;
						}

						bool GetBool(void) const
						{
							return m_Bool;
						}

						void SetBool(bool Value)
						{
							m_Type = Types::Boolean;
							m_Bool = Value;
						}

						float32 GetFloat32(void) const
						{
							return m_Float32;
						}

						void SetFloat32(float32 Value)
						{
							m_Type = Types::Float;
							m_Float32 = Value;
						}

					private:
						Types m_Type;

						union
						{
							bool m_Bool;
							float32 m_Float32;
						};
					};
				}
			}
		}
	}
}

#endif