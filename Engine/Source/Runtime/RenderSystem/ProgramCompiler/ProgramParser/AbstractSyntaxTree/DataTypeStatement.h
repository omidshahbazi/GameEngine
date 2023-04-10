// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DATA_TYPE_STATEMENT_H
#define DATA_TYPE_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>
#include <ProgramCompilerCommon\Common.h>
#include <ProgramCompilerCommon\ProgramCompilerException.h>
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

				bool IsWritableTexture(void) const
				{
					return IsWritableTexture(m_Type);
				}

				bool IsBuffer(void) const
				{
					return IsBuffer(m_Type);
				}

				bool IsWritableBuffer(void) const
				{
					return IsWritableBuffer(m_Type);
				}

				bool IsAllowedToDefineInGlobalScope(void)
				{
					if (!IsBuiltIn())
						return true;

					const ProgramDataTypes ALLOWED_CONTEXT_FREE_DATA_TYPES[]
					{
						ProgramDataTypes::Texture1D,
						ProgramDataTypes::Texture2D,
						ProgramDataTypes::Texture3D,
						ProgramDataTypes::TextureCube,
						ProgramDataTypes::Texture1DRW,
						ProgramDataTypes::Texture2DRW,
						ProgramDataTypes::Texture3DRW,
						ProgramDataTypes::Buffer,
						ProgramDataTypes::BufferRW
					};

					for (auto allowedType : ALLOWED_CONTEXT_FREE_DATA_TYPES)
					{
						if (allowedType != m_Type)
							continue;

						return true;
					}

					return false;
				}

				uint8 GetComponentCount(void) const
				{
					return GetComponentCount(m_Type);
				}

				String ToString(void) const
				{
					String result = "";

					if (IsBuiltIn())
						result = GetType(m_Type);
					else
						result = m_UserDefined;

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

				static String GetType(ProgramDataTypes Type)
				{
					switch (Type)
					{
					case ProgramDataTypes::Void:
						return "void";

					case ProgramDataTypes::Bool:
						return "bool";

					case ProgramDataTypes::Integer:
						return "int";

					case ProgramDataTypes::UnsignedInteger:
						return "uint";

					case ProgramDataTypes::Float:
						return "float";

					case ProgramDataTypes::Double:
						return "double";

					case ProgramDataTypes::Integer2:
						return "int2";

					case ProgramDataTypes::UnsignedInteger2:
						return "uint2";

					case ProgramDataTypes::Float2:
						return "float2";

					case ProgramDataTypes::Double2:
						return "double2";

					case ProgramDataTypes::Float3:
						return "float3";

					case ProgramDataTypes::Integer3:
						return "int3";

					case ProgramDataTypes::UnsignedInteger3:
						return "uint3";

					case ProgramDataTypes::Double3:
						return "double3";

					case ProgramDataTypes::Float4:
						return "float4";

					case ProgramDataTypes::Integer4:
						return "int4";

					case ProgramDataTypes::UnsignedInteger4:
						return "uint4";

					case ProgramDataTypes::Double4:
						return "double4";

					case ProgramDataTypes::Matrix4F:
						return "matrix4f";

					case ProgramDataTypes::Matrix4D:
						return "matrix4d";

					case ProgramDataTypes::Texture1D:
						return "texture1D";

					case ProgramDataTypes::Texture2D:
						return "texture2D";

					case ProgramDataTypes::Texture3D:
						return "texture3D";

					case ProgramDataTypes::TextureCube:
						return "textureCube";

					case ProgramDataTypes::Texture1DRW:
						return "texture1DRW";

					case ProgramDataTypes::Texture2DRW:
						return "texture2DRW";

					case ProgramDataTypes::Texture3DRW:
						return "texture3DRW";

					case ProgramDataTypes::Buffer:
						return "buffer";

					case ProgramDataTypes::BufferRW:
						return "bufferRW";
					}

					THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
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
						IsWritableTexture(Type));
				}

				static bool IsWritableTexture(ProgramDataTypes Type)
				{
					return (
						Type == ProgramDataTypes::Texture1DRW ||
						Type == ProgramDataTypes::Texture2DRW ||
						Type == ProgramDataTypes::Texture3DRW);
				}

				static bool IsBuffer(ProgramDataTypes Type)
				{
					return (
						Type == ProgramDataTypes::Buffer ||
						IsWritableBuffer(Type));
				}

				static bool IsWritableBuffer(ProgramDataTypes Type)
				{
					return (Type == ProgramDataTypes::BufferRW);
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