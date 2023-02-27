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
					m_TemplateElementDataType(nullptr),
					m_ElementCount(nullptr),
					m_PostElementCount(nullptr)
				{
				}

				DataTypeStatement(AllocatorBase* Allocator, const String& UserDefined) :
					m_Type(ProgramDataTypes::Unknown),
					m_TemplateElementDataType(nullptr),
					m_UserDefined(Allocator, UserDefined),
					m_ElementCount(nullptr),
					m_PostElementCount(nullptr)
				{
				}

				ProgramDataTypes GetType(void) const
				{
					return m_Type;
				}

				DataTypeStatement* GetTemplateElementDataType(void) const
				{
					return m_TemplateElementDataType;
				}

				void SetTemplateElementDataType(DataTypeStatement* Value)
				{
					m_TemplateElementDataType = Value;
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
					return IsIntegral(m_Type);
				}

				bool IsFloating(void) const
				{
					return IsFloating(m_Type);
				}

				bool IsNumeric(void) const
				{
					return IsNumeric(m_Type);
				}

				bool IsTexture(void) const
				{
					return IsTexture(m_Type);
				}

				uint8 GetComponentCount(void) const
				{
					return GetComponentCount(m_Type);
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

					case ProgramDataTypes::Texture1D:
						result = "texture1D";
						break;

					case ProgramDataTypes::Texture2D:
						result = "texture2D";
						break;

					case ProgramDataTypes::Texture3D:
						result = "texture3D";
						break;

					case ProgramDataTypes::TextureCube:
						result = "textureCube";
						break;

					case ProgramDataTypes::Texture1DRW:
						result = "texture1DRW";
						break;

					case ProgramDataTypes::Texture2DRW:
						result = "texture2DRW";
						break;

					case ProgramDataTypes::Texture3DRW:
						result = "texture3DRW";
						break;


					default:
						result = m_UserDefined;
						break;
					}

					if (m_TemplateElementDataType != nullptr)
					{
						result += '<';
						result += m_TemplateElementDataType->ToString();
						result += '>';
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

				static uint8 GetComponentCount(ProgramDataTypes Type)
				{
					switch (Type)
					{
					case ProgramDataTypes::Bool:
					case ProgramDataTypes::Integer:
					case ProgramDataTypes::UnsignedInteger:
					case ProgramDataTypes::Float:
					case ProgramDataTypes::Double:
						return 1;

					case ProgramDataTypes::Integer2:
					case ProgramDataTypes::UnsignedInteger2:
					case ProgramDataTypes::Float2:
					case ProgramDataTypes::Double2:
						return 2;

					case ProgramDataTypes::Float3:
					case ProgramDataTypes::Integer3:
					case ProgramDataTypes::UnsignedInteger3:
					case ProgramDataTypes::Double3:
						return 3;

					case ProgramDataTypes::Float4:
					case ProgramDataTypes::Integer4:
					case ProgramDataTypes::UnsignedInteger4:
					case ProgramDataTypes::Double4:
						return 4;

					case ProgramDataTypes::Matrix4F:
					case ProgramDataTypes::Matrix4D:
						return 16;
					}

					return 0;
				}

				static bool IsIntegral(ProgramDataTypes Type)
				{
					return (
						Type == ProgramDataTypes::Integer ||
						Type == ProgramDataTypes::UnsignedInteger ||
						Type == ProgramDataTypes::Integer2 ||
						Type == ProgramDataTypes::UnsignedInteger2 ||
						Type == ProgramDataTypes::Integer3 ||
						Type == ProgramDataTypes::UnsignedInteger3 ||
						Type == ProgramDataTypes::Integer4 ||
						Type == ProgramDataTypes::UnsignedInteger4);
				}

				static bool IsFloating(ProgramDataTypes Type)
				{
					return (
						Type == ProgramDataTypes::Float ||
						Type == ProgramDataTypes::Float2 ||
						Type == ProgramDataTypes::Float3 ||
						Type == ProgramDataTypes::Float4 ||
						Type == ProgramDataTypes::Double ||
						Type == ProgramDataTypes::Double2 ||
						Type == ProgramDataTypes::Double3 ||
						Type == ProgramDataTypes::Double4);
				}

				static bool IsNumeric(ProgramDataTypes Type)
				{
					return IsIntegral(Type) || IsFloating(Type);
				}

				static bool IsTexture(ProgramDataTypes Type)
				{
					return (
						Type == ProgramDataTypes::Texture1D ||
						Type == ProgramDataTypes::Texture2D ||
						Type == ProgramDataTypes::Texture3D ||
						Type == ProgramDataTypes::TextureCube ||
						Type == ProgramDataTypes::Texture1DRW ||
						Type == ProgramDataTypes::Texture2DRW ||
						Type == ProgramDataTypes::Texture3DRW);
				}

			private:
				ProgramDataTypes m_Type;
				DataTypeStatement* m_TemplateElementDataType;
				String m_UserDefined;
				Statement* m_ElementCount;
				Statement* m_PostElementCount;
			};
		}
	}
}

#endif