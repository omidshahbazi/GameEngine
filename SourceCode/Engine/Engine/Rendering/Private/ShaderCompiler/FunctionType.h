// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef FUNCTION_TYPE_H
#define FUNCTION_TYPE_H

#include <Rendering\Private\ShaderCompiler\ParameterType.h>
#include <Rendering\Private\ShaderCompiler\StatementsHolder.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class FunctionType : public Type, public StatementsHolder
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

					void SetReturnDataType(DataTypes Type)
					{
						m_ReturnType = Type;
					}

					DataTypes GetReturnDataType(void) const
					{
						return m_ReturnType;
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

						result += GetDataTypeName(m_ReturnType) + " " + GetName() + "(";

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

						for each (auto stm in GetStatements())
						{
							result += stm->ToString();
							result += "\n";
						}

						result += "}";

						return result;
					}

				private:
					DataTypes m_ReturnType;
					ParameterList m_Parameters;
					String m_Register;
					Types m_Type;
				};
			}
		}
	}
}

#endif