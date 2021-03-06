// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FUNCTION_TYPE_H
#define FUNCTION_TYPE_H

#include <Rendering\Private\ProgramCompiler\Syntax\ParameterType.h>
#include <Rendering\Private\ProgramCompiler\Syntax\Statement.h>

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
					class FunctionType : public Type, public StatementItemHolder
					{
					public:
						enum class Types
						{
							None = 0,
							VertexMain,
							TessellationMain,
							GeometryMain,
							FragmentMain,
							ComputeMain
						};

					public:
						FunctionType(AllocatorBase* Allocator) :
							StatementItemHolder(Allocator),
							m_Type(Types::None)
						{
						}

						virtual ~FunctionType(void)
						{
							for (auto parameter : m_Parameters)
								Destruct(parameter);
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

						bool IsEntrypoint(void) const
						{
							return (m_Type != Types::None);
						}

						String ToString(void) const override
						{
							String result;

							result += m_ReturnType.ToString() + " " + GetName() + "(";

							bool isFirst = true;
							for (auto par : m_Parameters)
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

							for (auto stm : GetItems())
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

					typedef Vector<FunctionType*> FunctionList;
				}
			}
		}
	}
}

#endif