// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef AST_STRUCT_TYPE_H
#define AST_STRUCT_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\StructVariableType.h>
#include <ProgramParser\AbstractSyntaxTree\SyntaxItemHolder.h>
#include <RenderCommon\GPUAlignedType.h>
#include <RenderCommon\Enums.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Containers;
	using namespace RenderCommon;
	using namespace Debugging;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API StructType : public Type, public SyntaxItemHolder<StructVariableType>
			{
			public:
				StructType(AllocatorBase* Allocator) :
					Type(Allocator),
					SyntaxItemHolder<StructVariableType>(Allocator)
				{
				}

				virtual String ToString(void) const override
				{
					String result;

					result += "struct " + GetName();
					result += "\n{";

					for (auto& variable : GetItems())
						result += "\n" + variable->ToString();

					result += "\n};";

					return result;
				}

				static uint16 GetAlignedSize(uint16 Size, uint8 Alignment)
				{
					if (Size % Alignment != 0)
						Size = ((Size / Alignment) + 1) * Alignment;

					return Size;
				}

				static void GetAlignedOffset(ProgramDataTypes DataType, uint16& Offset, uint8& Size)
				{
					uint8 alignment = 0;
					switch (DataType)
					{
					case ProgramDataTypes::Bool:
						Size = GPUAlignedBool::Size;
						alignment = GPUAlignedBool::Alignment;
						break;

					case ProgramDataTypes::Integer:
						Size = GPUAlignedInt32::Size;
						alignment = GPUAlignedInt32::Alignment;
						break;

					case ProgramDataTypes::UnsignedInteger:
						Size = GPUAlignedUInt32::Size;
						alignment = GPUAlignedUInt32::Alignment;
						break;

					case ProgramDataTypes::Float:
						Size = GPUAlignedFloat32::Size;
						alignment = GPUAlignedFloat32::Alignment;
						break;

					case ProgramDataTypes::Double:
						Size = GPUAlignedFloat64::Size;
						alignment = GPUAlignedFloat64::Alignment;
						break;

					case ProgramDataTypes::Integer2:
						Size = GPUAlignedVector2I::Size;
						alignment = GPUAlignedVector2I::Alignment;
						break;

					case ProgramDataTypes::UnsignedInteger2:
						Size = GPUAlignedVector2U::Size;
						alignment = GPUAlignedVector2U::Alignment;
						break;

					case ProgramDataTypes::Float2:
						Size = GPUAlignedVector2F::Size;
						alignment = GPUAlignedVector2F::Alignment;
						break;

					case ProgramDataTypes::Double2:
						Size = GPUAlignedVector2D::Size;
						alignment = GPUAlignedVector2D::Alignment;
						break;

					case ProgramDataTypes::Integer3:
						Size = GPUAlignedVector3I::Size;
						alignment = GPUAlignedVector3I::Alignment;
						break;

					case ProgramDataTypes::UnsignedInteger3:
						Size = GPUAlignedVector3U::Size;
						alignment = GPUAlignedVector3U::Alignment;
						break;

					case ProgramDataTypes::Float3:
						Size = GPUAlignedVector3F::Size;
						alignment = GPUAlignedVector3F::Alignment;
						break;

					case ProgramDataTypes::Double3:
						Size = GPUAlignedVector3D::Size;
						alignment = GPUAlignedVector3D::Alignment;
						break;

					case ProgramDataTypes::Integer4:
						Size = GPUAlignedVector4I::Size;
						alignment = GPUAlignedVector4I::Alignment;
						break;

					case ProgramDataTypes::UnsignedInteger4:
						Size = GPUAlignedVector4U::Size;
						alignment = GPUAlignedVector4U::Alignment;
						break;

					case ProgramDataTypes::Float4:
						Size = GPUAlignedVector4F::Size;
						alignment = GPUAlignedVector4F::Alignment;
						break;

					case ProgramDataTypes::Double4:
						Size = GPUAlignedVector4D::Size;
						alignment = GPUAlignedVector4D::Alignment;
						break;

					case ProgramDataTypes::Matrix4F:
						Size = GPUAlignedMatrix4F::Size;
						alignment = GPUAlignedMatrix4F::Alignment;
						break;

					case ProgramDataTypes::Matrix4D:
						Size = GPUAlignedMatrix4D::Size;
						alignment = GPUAlignedMatrix4D::Alignment;
						break;

					default:
						THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
					}

					Offset = GetAlignedSize(Offset, alignment);
				}

				static uint16 GetStructSize(const StructType* Struct)
				{
					uint16 totalSize = 0;

					for (auto& variableType : Struct->GetItems())
					{
						ProgramDataTypes dataType = variableType->GetDataType()->GetType();

						uint8 size = 0;
						GetAlignedOffset(dataType, totalSize, size);

						totalSize += size;
					}

					return GetAlignedSize(totalSize, 16);
				}
			};

			typedef Vector<StructType*> StructList;
		}
	}
}

#endif