// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef VARIABLE_TYPE_H
#define VARIABLE_TYPE_H

#include <Rendering\Private\ShaderCompiler\Type.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class VariableType : public Type
				{
				public:
					VariableType(void)
					{
					}

					void SetDataType(DataTypes DataType)
					{
						m_DataType = DataType;
					}

					DataTypes GetDataType(void) const
					{
						return m_DataType;
					}

					void SetRegister(const String &Register)
					{
						m_Register = Register;
					}

					const String &GetRegister(void) const
					{
						return m_Register;
					}

					String ToString(void) const override
					{
						String result;

						result += GetDataTypeName(m_DataType) + " " + GetName();

						if (m_Register.GetLength() != 0)
							result += " : " + m_Register;

						result += ";";

						return result;
					}

				private:
					DataTypes m_DataType;
					String m_Register;
				};
			}
		}
	}
}

#endif