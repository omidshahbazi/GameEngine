// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PARAMETER_TYPE_H
#define PARAMETER_TYPE_H

#include <Rendering\Private\ShaderCompiler\Syntax\Type.h>
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
				namespace Syntax
				{
					class ParameterType : public Type
					{
					public:
						ParameterType(void)
						{
						}

						void SetDataType(const ShaderDataType& Type)
						{
							m_DataType = Type;
						}

						const ShaderDataType& GetDataType(void) const
						{
							return m_DataType;
						}

						void SetRegister(const String& Register)
						{
							m_Register = Register;
						}

						const String& GetRegister(void) const
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
						ShaderDataType m_DataType;
						String m_Register;
					};

					typedef Vector<ParameterType*> ParameterList;
				}
			}
		}
	}
}

#endif