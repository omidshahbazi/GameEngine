// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PARAMETER_TYPE_H
#define PARAMETER_TYPE_H

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
				class ParameterType : public Type
				{
				public:
					ParameterType(void)
					{
					}

					void SetDataType(const DataType &Type)
					{
						m_DataType = Type;
					}

					const DataType &GetDataType(void) const
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

						result += m_DataType.ToString() + " " + GetName();

						if (m_Register.GetLength() != 0)
							result += " : " + m_Register;

						return result;
					}

				private:
					DataType m_DataType;
					String m_Register;
				};

				typedef Vector<ParameterType*> ParameterList;
			}
		}
	}
}

#endif