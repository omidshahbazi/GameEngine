// Copyright 2016-2017 ?????????????. All Rights Reserved.
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

					void SetTypeName(const String &TypeName)
					{
						m_TypeName = TypeName;
					}

					const String &GetTypeName(void) const
					{
						return m_TypeName;
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
					String m_TypeName;
					String m_Register;
				};

				typedef Vector<ParameterType*> ParameterList;
			}
		}
	}
}

#endif