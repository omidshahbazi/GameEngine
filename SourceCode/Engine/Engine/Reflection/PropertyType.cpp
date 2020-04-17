// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\PropertyType.h>

namespace Engine
{
	namespace Reflection
	{
		PropertyType::PropertyType(Type* TopNest) :
			Type(TopNest),
			m_Offset(0)
		{
		}

		void PropertyType::SetValue(void* TargetObject, const AnyDataType& Value)
		{
			if (m_DataType.GetIsConstValue())
				return;

			if (m_DataType.GetValueType() != Value.GetValueType())
				return;

			uint8 size = 0;

			switch (m_DataType.GetValueType())
			{
			case ValueTypes::Bool:
			case ValueTypes::Int8:
			case ValueTypes::Int16:
			case ValueTypes::Int32:
			case ValueTypes::Int64:
			case ValueTypes::UInt8:
			case ValueTypes::UInt16:
			case ValueTypes::UInt32:
			case ValueTypes::UInt64:
			case ValueTypes::Float32:
			case ValueTypes::Float64:
			case ValueTypes::ColorUI8:
			case ValueTypes::Vector2F:
			case ValueTypes::Vector2I:
			case ValueTypes::Vector3F:
			case ValueTypes::Vector4F:
			case ValueTypes::Matrix4F:
				size = Value.GetDataSize();
				PlatformMemory::Copy(Value.GetPointer<byte>(), (byte*)TargetObject + m_Offset, size);
				break;

			case ValueTypes::String:
			case ValueTypes::WString:
				break;
			}
		}
	}
}