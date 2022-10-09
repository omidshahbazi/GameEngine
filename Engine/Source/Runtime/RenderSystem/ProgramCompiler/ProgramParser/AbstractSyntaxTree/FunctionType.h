// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef AST_FUNCTION_TYPE_H
#define AST_FUNCTION_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\ParameterType.h>
#include <ProgramParser\AbstractSyntaxTree\BaseAttributeType.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API FunctionType : public Type, public StatementItemHolder
			{
			public:
				enum class Types
				{
					None = 0,
					VertexMain,
					HullMain,
					DomainMain,
					GeometryMain,
					FragmentMain,
					ComputeMain
				};

			public:
				FunctionType(AllocatorBase* Allocator) :
					Type(Allocator),
					StatementItemHolder(Allocator),
					m_ReturnType(nullptr),
					m_Parameters(Allocator),
					m_Type(Types::None)
				{
				}

				virtual ~FunctionType(void)
				{
					if (m_ReturnType != nullptr)
						Destruct(m_ReturnType);

					for (auto parameter : m_Parameters)
						Destruct(parameter);
				}

				void AddAttributes(const AttributeList& Attributes)
				{
					m_Attributes.AddRange(Attributes);
				}

				const AttributeList& GetAttributes(void) const
				{
					return m_Attributes;
				}

				template<typename T>
				const T* GetAttribute(void) const
				{
					int32 index = m_Attributes.FindIf([](auto attributeType) { return IsTypeOf(attributeType, T); });
					if (index == -1)
						return nullptr;

					return ReinterpretCast(const T*, m_Attributes[index]);
				}

				void SetReturnDataType(DataTypeStatement* Type)
				{
					m_ReturnType = Type;
				}

				DataTypeStatement* GetReturnDataType(void) const
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

				virtual String ToString(void) const override
				{
					String result;

					for (auto attr : m_Attributes)
					{
						result += attr->ToString();
						result += "\n";
					}

					result += (m_ReturnType == nullptr ? "void" : m_ReturnType->ToString()) + " " + GetName() + "(";

					bool isFirst = true;
					for (auto par : m_Parameters)
					{
						if (!isFirst)
							result += ", ";

						isFirst = false;

						result += par->ToString();
					}

					result += ")";

					result += "\n{\n";

					for (auto stm : GetItems())
					{
						result += stm->ToString();
						result += "\n";
					}

					result += "}";

					return result;
				}

			private:
				AttributeList m_Attributes;
				DataTypeStatement* m_ReturnType;
				ParameterList m_Parameters;
				Types m_Type;
			};

			typedef Vector<FunctionType*> FunctionList;
		}
	}
}

#endif