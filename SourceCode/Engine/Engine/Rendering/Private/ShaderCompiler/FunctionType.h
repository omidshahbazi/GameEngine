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
					enum class Types
					{
						None = 0,
						VertexMain,
						FragmentMain
					};

				public:
					FunctionType(void) :
						m_Type(Types::None)
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

					void SetType(Types Type)
					{
						m_Type = Type;
					}

					Types GetType(void) const
					{
						return m_Type;
					}

					String ToString(void) const override
					{
						String result;

						result += m_ReturnTypeName + " " + GetName() + "(";

						bool isFirst = true;
						for each (auto par in m_Parameters)
						{
							if (!isFirst)
								result += ", ";

							isFirst = false;

							result += par->ToString();
						}

						result += ")";

						if (m_Register.GetLength() != 0)
							result += " : " + m_Register;

						result += "\n{";

						result += "}";

						return result;
					}

				private:
					String m_ReturnTypeName;
					ParameterList m_Parameters;
					String m_Register;
					Types m_Type;
				};
			}
		}
	}
}

#endif