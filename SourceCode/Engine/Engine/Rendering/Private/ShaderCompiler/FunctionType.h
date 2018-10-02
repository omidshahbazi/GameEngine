// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef FUNCTION_TYPE_H
#define FUNCTION_TYPE_H

#include <Rendering\Private\ShaderCompiler\ParameterType.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class FunctionType : public Type
				{
				public:
					FunctionType(void)
					{
					}

					void SetReturnDataType(const String &TypeName)
					{
						m_ReturnTypeName = TypeName;
					}

					const String &GetReturnDataType(void) const
					{
						return m_ReturnTypeName;
					}

					void AddParamaeter(ParameterType *Parameter)
					{
						m_Parameters.Add(Parameter);
					}

					const ParameterList &GetParameters(void) const
					{
						return m_Parameters;
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
					String m_ReturnTypeName;
					ParameterList m_Parameters;
					String m_Register;
				};
			}
		}
	}
}

#endif