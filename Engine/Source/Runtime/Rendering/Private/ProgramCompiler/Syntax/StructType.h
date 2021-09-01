// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef STRUCT_TYPE_H
#define STRUCT_TYPE_H

#include <Rendering\Private\ProgramCompiler\Syntax\VariableType.h>
#include <Rendering\Private\ProgramCompiler\Syntax\SyntaxItemHolder.h>
#include <Rendering\GPUAlignedType.h>

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
					class StructType : public Type, public SyntaxItemHolder<VariableType>
					{
					public:
						StructType(AllocatorBase* Allocator) :
							Type(Allocator),
							SyntaxItemHolder<VariableType>(Allocator)
						{
						}

						String ToString(void) const override
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

							case ProgramDataTypes::Float:
								Size = GPUAlignedFloat32::Size;
								alignment = GPUAlignedFloat32::Alignment;
								break;

							case ProgramDataTypes::Double:
								Size = GPUAlignedFloat64::Size;
								alignment = GPUAlignedFloat64::Alignment;
								break;

							case ProgramDataTypes::Float2:
								Size = GPUAlignedVector2F::Size;
								alignment = GPUAlignedVector2F::Alignment;
								break;

							case ProgramDataTypes::Double2:
								Size = GPUAlignedVector2D::Size;
								alignment = GPUAlignedVector2D::Alignment;
								break;

							case ProgramDataTypes::Float3:
								Size = GPUAlignedVector3F::Size;
								alignment = GPUAlignedVector3F::Alignment;
								break;

							case ProgramDataTypes::Double3:
								Size = GPUAlignedVector3D::Size;
								alignment = GPUAlignedVector3D::Alignment;
								break;

							case ProgramDataTypes::Float4:
								Size = GPUAlignedVector4F::Size;
								alignment = GPUAlignedVector4F::Alignment;
								break;

							case ProgramDataTypes::Double4:
								Size = GPUAlignedVector4D::Size;
								alignment = GPUAlignedVector4D::Alignment;
								break;

							case ProgramDataTypes::Matrix4:
								Size = GPUAlignedMatrix4F::Size;
								alignment = GPUAlignedMatrix4F::Alignment;
								break;
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
	}
}

#endif