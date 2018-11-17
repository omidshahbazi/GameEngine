// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONSTANT_STATEMENT_H
#define CONSTANT_STATEMENT_H

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
				class ConstantStatement : public Statement
				{
				public:
					ConstantStatement(void)
					{
					}

					Types GetType(void) const override
					{
						return Types::Constant;
					}

					float GetValue(void) const
					{
						return m_Value;
					}

					void SetValue(float Value)
					{
						m_Value = Value;
					}

				private:
					float m_Value;
				};
			}
		}
	}
}

#endif