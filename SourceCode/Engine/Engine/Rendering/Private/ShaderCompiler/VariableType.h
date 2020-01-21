// Copyright 2016-2020 ?????????????. All Rights Reserved.
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
					VariableType(void) :
						m_IsConstant(false)
					{
					}

					const DataType &GetDataType(void) const
					{
						return m_DataType;
					}

					void SetDataType(const DataType &DataType)
					{
						m_DataType = DataType;
					}

					const String &GetRegister(void) const
					{
						return m_Register;
					}

					void SetRegister(const String &Register)
					{
						m_Register = Register;
					}

					bool GetIsConstant(void) const
					{
						return m_IsConstant;
					}

					void SetIsConstant(bool Value)
					{
						m_IsConstant = Value;
					}

					String ToString(void) const override
					{
						String result;

						result += m_DataType.ToString() + " " + GetName();

						if (m_Register.GetLength() != 0)
							result += " : " + m_Register;

						result += ";";

						return result;
					}

				private:
					DataType m_DataType;
					String m_Register;
					bool m_IsConstant;
				};
			}
		}
	}
}

#endif