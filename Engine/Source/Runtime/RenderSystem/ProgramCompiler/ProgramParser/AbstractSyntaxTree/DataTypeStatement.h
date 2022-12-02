// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DATA_TYPE_STATEMENT_H
#define DATA_TYPE_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>
#include <ProgramCompilerCommon\Common.h>
#include <RenderCommon\Enums.h>
#include <Containers\Strings.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Containers;
	using namespace RenderCommon;
	using namespace ProgramCompilerCommon;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API DataTypeStatement : public Statement
			{
			public:
				DataTypeStatement(ProgramDataTypes Type) :
					m_Type(Type),
					m_ElementCount(nullptr),
					m_PostElementCount(nullptr)
				{
				}

				DataTypeStatement(AllocatorBase* Allocator, const String& UserDefined) :
					m_Type(ProgramDataTypes::Unknown),
					m_UserDefined(Allocator, UserDefined),
					m_ElementCount(nullptr),
					m_PostElementCount(nullptr)
				{
				}

				ProgramDataTypes GetType(void) const
				{
					return m_Type;
				}

				const String& GetUserDefined(void) const
				{
					return m_UserDefined;
				}

				Statement* GetElementCount(void) const
				{
					return m_ElementCount;
				}

				void SetElementCount(Statement* Value)
				{
					m_ElementCount = Value;
				}

				Statement* GetPostElementCount(void) const
				{
					return m_PostElementCount;
				}

				void SetPostElementCount(Statement* Value)
				{
					m_PostElementCount = Value;
				}

				bool IsBuiltIn(void) const
				{
					return (m_Type != ProgramDataTypes::Unknown);
				}

				bool IsArray(void) const
				{
					return !(m_ElementCount == nullptr && m_PostElementCount == nullptr);
				}

				bool IsIntegral(void) const
				{
					return (
						m_Type == ProgramDataTypes::Integer ||
						m_Type == ProgramDataTypes::UnsignedInteger ||
						m_Type == ProgramDataTypes::Integer2 ||
						m_Type == ProgramDataTypes::UnsignedInteger2 ||
						m_Type == ProgramDataTypes::Integer3 ||
						m_Type == ProgramDataTypes::UnsignedInteger3 ||
						m_Type == ProgramDataTypes::Integer4 ||
						m_Type == ProgramDataTypes::UnsignedInteger4);
				}

				bool IsFloating(void) const
				{
					return (
						m_Type == ProgramDataTypes::Float ||
						m_Type == ProgramDataTypes::Float2 ||
						m_Type == ProgramDataTypes::Float3 ||
						m_Type == ProgramDataTypes::Float4 ||
						m_Type == ProgramDataTypes::Double ||
						m_Type == ProgramDataTypes::Double2 ||
						m_Type == ProgramDataTypes::Double3 ||
						m_Type == ProgramDataTypes::Double4);
				}

				bool IsNumeric(void) const
				{
					return IsIntegral() || IsFloating();
				}

				String ToString(void) const
				{
					String result = "";

					switch (m_Type)
					{
					case ProgramDataTypes::Void:
						result = "void";
						break;

					case ProgramDataTypes::Bool:
						result = "bool";
						break;

					case ProgramDataTypes::Integer:
						result = "int";
						break;

					case ProgramDataTypes::UnsignedInteger:
						result = "uint";
						break;

					case ProgramDataTypes::Float:
						result = "float";
						break;

					case ProgramDataTypes::Double:
						result = "double";
						break;

					case ProgramDataTypes::Integer2:
						result = "int2";
						break;

					case ProgramDataTypes::UnsignedInteger2:
						result = "uint2";
						break;

					case ProgramDataTypes::Float2:
						result = "float2";
						break;

					case ProgramDataTypes::Double2:
						result = "double2";
						break;

					case ProgramDataTypes::Float3:
						result = "float3";
						break;

					case ProgramDataTypes::Integer3:
						result = "int3";
						break;

					case ProgramDataTypes::UnsignedInteger3:
						result = "uint3";
						break;

					case ProgramDataTypes::Double3:
						result = "double3";
						break;

					case ProgramDataTypes::Float4:
						result = "float4";
						break;

					case ProgramDataTypes::Integer4:
						result = "int4";
						break;

					case ProgramDataTypes::UnsignedInteger4:
						result = "uint4";
						break;

					case ProgramDataTypes::Double4:
						result = "double4";
						break;

					case ProgramDataTypes::Matrix4F:
						result = "matrix4f";
						break;

					case ProgramDataTypes::Matrix4D:
						result = "matrix4d";
						break;

					default:
						result = m_UserDefined;
						break;
					}

					if (m_ElementCount != nullptr)
					{
						result += "[";
						result += m_ElementCount->ToString();
						result += "]";
					}

					return result;
				}

				String PostElementCountToString(void) const
				{
					String result = "";

					if (m_PostElementCount != nullptr)
					{
						result += "[";
						result += m_PostElementCount->ToString();
						result += "]";
					}

					return result;
				}

			private:
				ProgramDataTypes m_Type;
				String m_UserDefined;
				Statement* m_ElementCount;
				Statement* m_PostElementCount;
			};
		}
	}
}

#endif