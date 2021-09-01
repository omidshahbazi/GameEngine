// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef VARIABLE_TYPE_H
#define VARIABLE_TYPE_H

#include <Rendering\Private\ProgramCompiler\Syntax\Type.h>
#include <Rendering\Private\ProgramCompiler\Syntax\DataTypeStatement.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				namespace Syntax
				{
					class VariableType : public Type
					{
					public:
						VariableType(AllocatorBase* Allocator) :
							Type(Allocator),
							m_DataType(nullptr),
							m_Register(Allocator)
						{
						}

						~VariableType(void)
						{
							Destruct(m_DataType);
						}

						DataTypeStatement* GetDataType(void) const
						{
							return m_DataType;
						}

						void SetDataType(DataTypeStatement* DataType)
						{
							m_DataType = DataType;
						}

						const String& GetRegister(void) const
						{
							return m_Register;
						}

						void SetRegister(const String& Register)
						{
							m_Register = Register;
						}

						String ToString(void) const override
						{
							String result;

							result += m_DataType->ToString() + " " + GetName();

							if (m_Register.GetLength() != 0)
								result += " : " + m_Register;

							result += ";";

							return result;
						}

					private:
						DataTypeStatement* m_DataType;
						String m_Register;
					};

					typedef Vector<VariableType*> VariableList;
				}
			}
		}
	}
}

#endif