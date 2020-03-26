// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FUNCTION_TYPE_H
#define FUNCTION_TYPE_H

#include <Rendering\Private\ShaderCompiler\Syntax\ParameterType.h>
#include <Rendering\Private\ShaderCompiler\Syntax\StatementsHolder.h>

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

						void SetReturnDataType(const DataType& Type)
						{
							m_ReturnType = Type;
						}

						const DataType& GetReturnDataType(void) const
						{
							return m_ReturnType;
						}

						void AddParamaeter(ParameterType* Parameter)
						{
							m_Parameters.Add(Parameter);
						}

						const ParameterList& GetParameters(void) const
						{
							return m_Parameters;
						}

						void SetRegister(const String& Register)
						{
							m_Register = Register;
						}

						const String& GetRegister(void) const
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

							result += m_ReturnType.ToString() + " " + GetName() + "(";

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
						DataType m_ReturnType;
						ParameterList m_Parameters;
						String m_Register;
						Types m_Type;
					};
				}
			}
		}
	}
}

#endif