// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef VARIABLE_TYPE_H
#define VARIABLE_TYPE_H

#include <Containers\Strings.h>
#include <Containers\Vector.h>
#include <Rendering\Private\ShaderCompiler\DataTypes.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class VariableType
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

					void SetName(const String &Name)
					{
						m_Name = Name;
					}

					const String &GetName(void) const
					{
						return m_Name;
					}

					void SetRegister(const String &Register)
					{
						m_Register = Register;
					}

					const String &GetRegister(void) const
					{
						return m_Register;
					}

				private:
					DataTypes m_DataType;
					String m_Name;
					String m_Register;
				};

				typedef Vector<VariableType*> VariableList;
			}
		}
	}
}

#endif