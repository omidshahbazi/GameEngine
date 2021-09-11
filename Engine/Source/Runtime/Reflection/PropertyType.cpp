// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\PropertyType.h>
#include <Reflection\Enum.h>

namespace Engine
{
	namespace Reflection
	{

		PropertyType::PropertyType(Type* TopNest) :
			Type(TopNest),
			m_Offset(0)
		{
		}

		AnyDataType PropertyType::GetValue(void* TargetObject) const
		{
			byte* target = (byte*)TargetObject + m_Offset;

			if (m_DataType.GetExtraValueType().GetLength() != 0)
				if (Enum::GetType(m_DataType.GetExtraValueType()) != nullptr)
					return *ReinterpretCast(int32*, target);

			switch (m_DataType.GetValueType())
			{
			case ValueTypes::VoidPointer:
				return ReinterpretCast(void*, target);
			case ValueTypes::Bool:
				return *ReinterpretCast(bool*, target);
			case ValueTypes::Int8:
				return *ReinterpretCast(int8*, target);
			case ValueTypes::Int16:
				return *ReinterpretCast(int16*, target);
			case ValueTypes::Int32:
				return *ReinterpretCast(int32*, target);
			case ValueTypes::Int64:
				return *ReinterpretCast(int64*, target);
			case ValueTypes::UInt8:
				return *ReinterpretCast(uint8*, target);
			case ValueTypes::UInt16:
				return *ReinterpretCast(uint16*, target);
			case ValueTypes::UInt32:
				return *ReinterpretCast(uint32*, target);
			case ValueTypes::UInt64:
				return *ReinterpretCast(uint64*, target);
			case ValueTypes::Float32:
				return *ReinterpretCast(float32*, target);
			case ValueTypes::Float64:
				return *ReinterpretCast(float64*, target);
			case ValueTypes::String:
				return *ReinterpretCast(String*, target);
			case ValueTypes::WString:
				return *ReinterpretCast(WString*, target);
			case ValueTypes::ColorUI8:
				return *ReinterpretCast(ColorUI8*, target);
			case ValueTypes::Vector2F:
				return *ReinterpretCast(Vector2F*, target);
			case ValueTypes::Vector2I:
				return *ReinterpretCast(Vector2I*, target);
			case ValueTypes::Vector3F:
				return *ReinterpretCast(Vector3F*, target);
			case ValueTypes::Vector4F:
				return *ReinterpretCast(Vector4F*, target);
			case ValueTypes::Matrix4F:
				return *ReinterpretCast(Matrix4F*, target);
			}

			return AnyDataType();
		}

		void PropertyType::SetValue(void* TargetObject, const AnyDataType& Value)
		{
			if (m_DataType.GetIsConstValue())
				return;

			byte* target = (byte*)TargetObject + m_Offset;
			byte* newValue = ReinterpretCast(byte*, Value.GetPointer<byte>());

			bool isNumberValueType = AnyDataType::IsNumericValueType(Value.GetValueType());

			if (m_DataType.GetExtraValueType().GetLength() != 0)
			{
				int32 enumValue = 0;

				if (Value.GetValueType() == ValueTypes::String)
					enumValue = Enum::Parse(m_DataType.GetExtraValueType(), Value.Get<String>());
				else if (isNumberValueType)
					enumValue = *ReinterpretCast(int32*, newValue);
				else
					return;

				PlatformMemory::Copy(ReinterpretCast(byte*, &enumValue), target, sizeof(int32));

				return;
			}

			if (isNumberValueType)
			{
				PlatformMemory::Copy(newValue, target, m_DataType.GetDataSize());
				return;
			}

			switch (m_DataType.GetValueType())
			{
			case ValueTypes::String:
			{
				String* str = ReinterpretCast(String*, target);
				*str = Value.GetAsString();
			} break;

			case ValueTypes::WString:
			{
				WString* str = ReinterpretCast(WString*, target);
				*str = Value.GetAsWString();
			} break;
			}
		}
	}
}