// Copyright 2012-2015 ?????????????. All Rights Reserved.
#ifndef ANY_DATA_TYPE_H
#define ANY_DATA_TYPE_H

#include <Common\PrimitiveTypes.h>
#include <Common\ValueTypes.h>
#include <Containers\Strings.h>
#include <Containers\MathContainers.h>

namespace Engine
{
	using namespace Common;

	namespace Containers
	{
		struct AnyDataType
		{
		private:
			union Data
			{
			public:
				Data(void)
				{
					VoidPointer = nullptr;
				}

				~Data(void)
				{
				}

				void *VoidPointer;

				bool Bool;

				int8 Int8;
				int16 Int16;
				int32 Int32;
				int64 Int64;

				uint8 UInt8;
				uint16 UInt16;
				uint32 UInt32;
				uint64 UInt64;

				float32 Float32;
				float64 Float64;

				String String;
				WString WString;

				Vector2F Vector2F;
				Vector3F Vector3F;
				Matrix4F Matrix4F;
			};

		public:
			AnyDataType(void) :
				m_ValueType(ValueTypes::None)
			{
			}

			AnyDataType(const AnyDataType &Other) :
				m_ValueType(ValueTypes::None)
			{
				*this = Other;
			}

			AnyDataType(void *Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(bool Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(int8 Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(int16 Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(int32 Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(int64 Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(uint8 Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(uint16 Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(uint32 Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(uint64 Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(float32 Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(float64 Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(const String &Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(const WString &Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(const Vector2F &Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(const Vector3F &Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(const Matrix4F &Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			INLINE AnyDataType &operator= (const AnyDataType &Other)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == Other.m_ValueType, "New value should have same type as other's value");

				PlatformMemory::Copy(&Other.m_Data, &m_Data, sizeof(Data));
				m_ValueType = Other.m_ValueType;

				return *this;
			}

			INLINE AnyDataType &operator= (void *Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::VoidPointer, "Value types are mismatched");

				m_Data.VoidPointer = Value;
				m_ValueType = ValueTypes::VoidPointer;

				return *this;
			}

			INLINE AnyDataType &operator= (bool Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Bool, "Value types are mismatched");

				m_Data.Bool = Value;
				m_ValueType = ValueTypes::Bool;

				return *this;
			}

			INLINE AnyDataType &operator= (int8 Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Int8, "Value types are mismatched");

				m_Data.Int8 = Value;
				m_ValueType = ValueTypes::Int8;

				return *this;
			}

			INLINE AnyDataType &operator= (int16 Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Int16, "Value types are mismatched");

				m_Data.Int16 = Value;
				m_ValueType = ValueTypes::Int16;

				return *this;
			}

			INLINE AnyDataType &operator= (int32 Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Int32, "Value types are mismatched");

				m_Data.Int32 = Value;
				m_ValueType = ValueTypes::Int32;

				return *this;
			}

			INLINE AnyDataType &operator= (int64 Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Int64, "Value types are mismatched");

				m_Data.Int64 = Value;
				m_ValueType = ValueTypes::Int64;

				return *this;
			}

			INLINE AnyDataType &operator= (uint8 Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::UInt8, "Value types are mismatched");

				m_Data.UInt8 = Value;
				m_ValueType = ValueTypes::UInt8;

				return *this;
			}

			INLINE AnyDataType &operator= (uint16 Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::UInt16, "Value types are mismatched");

				m_Data.UInt16 = Value;
				m_ValueType = ValueTypes::UInt16;

				return *this;
			}

			INLINE AnyDataType &operator= (uint32 Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::UInt32, "Value types are mismatched");

				m_Data.UInt32 = Value;
				m_ValueType = ValueTypes::UInt32;

				return *this;
			}

			INLINE AnyDataType &operator= (uint64 Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::UInt64, "Value types are mismatched");

				m_Data.UInt64 = Value;
				m_ValueType = ValueTypes::UInt64;

				return *this;
			}

			INLINE AnyDataType &operator= (float32 Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Float32, "Value types are mismatched");

				m_Data.Float32 = Value;
				m_ValueType = ValueTypes::Float32;

				return *this;
			}

			INLINE AnyDataType &operator= (float64 Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Float64, "Value types are mismatched");

				m_Data.Float64 = Value;
				m_ValueType = ValueTypes::Float64;

				return *this;
			}

			INLINE AnyDataType &operator= (const String &Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::String, "Value types are mismatched");

				m_Data.String = Value;
				m_ValueType = ValueTypes::String;

				return *this;
			}

			INLINE AnyDataType &operator= (const WString &Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::WString, "Value types are mismatched");

				m_Data.WString = Value;
				m_ValueType = ValueTypes::WString;

				return *this;
			}

			INLINE AnyDataType &operator= (const Vector2F &Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::WString, "Value types are mismatched");

				m_Data.Vector2F = Value;
				m_ValueType = ValueTypes::Vector2F;

				return *this;
			}

			INLINE AnyDataType &operator= (const Vector3F &Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::WString, "Value types are mismatched");

				m_Data.Vector3F = Value;
				m_ValueType = ValueTypes::Vector3F;

				return *this;
			}

			INLINE AnyDataType &operator= (const Matrix4F &Value)
			{
				Assert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::WString, "Value types are mismatched");

				m_Data.Matrix4F = Value;
				m_ValueType = ValueTypes::Matrix4F;

				return *this;
			}

			template<typename T>
			INLINE T Get(void) const
			{
				return *(ReinterpretCast(T*, ReinterpretCast(void*, ConstCast(Data*, &m_Data))));
			}

			INLINE void *GetAsVoid(void) const
			{
				Assert(m_ValueType == ValueTypes::VoidPointer, "Value type is different");

				return m_Data.VoidPointer;
			}

			INLINE bool GetAsBool(void) const
			{
				Assert(m_ValueType == ValueTypes::Bool, "Value type is different");

				return m_Data.Bool;
			}

			INLINE int8 GetAsInt8(void) const
			{
				Assert(m_ValueType == ValueTypes::Int8, "Value type is different");

				return m_Data.Int8;
			}

			INLINE int16 GetAsInt16(void) const
			{
				Assert(m_ValueType == ValueTypes::Int16, "Value type is different");

				return m_Data.Int16;
			}

			INLINE int32 GetAsInt32(void) const
			{
				Assert(m_ValueType == ValueTypes::Int32, "Value type is different");

				return m_Data.Int32;
			}

			INLINE int64 GetAsInt64(void) const
			{
				Assert(m_ValueType == ValueTypes::Int64, "Value type is different");

				return m_Data.Int64;
			}

			INLINE uint8 GetAsUInt8(void) const
			{
				Assert(m_ValueType == ValueTypes::UInt8, "Value type is different");

				return m_Data.UInt8;
			}

			INLINE uint16 GetAsUInt16(void) const
			{
				Assert(m_ValueType == ValueTypes::UInt16, "Value type is different");

				return m_Data.UInt16;
			}

			INLINE uint32 GetAsUInt32(void) const
			{
				Assert(m_ValueType == ValueTypes::UInt32, "Value type is different");

				return m_Data.UInt32;
			}

			INLINE uint64 GetAsUInt64(void) const
			{
				Assert(m_ValueType == ValueTypes::UInt64, "Value type is different");

				return m_Data.UInt64;
			}

			INLINE float32 GetAsFloat32(void) const
			{
				Assert(m_ValueType == ValueTypes::Float32, "Value type is different");

				return m_Data.Float32;
			}

			INLINE float64 GetAsFloat64(void) const
			{
				Assert(m_ValueType == ValueTypes::Float64, "Value type is different");

				return m_Data.Float64;
			}

			INLINE const String &GetAsString(void) const
			{
				Assert(m_ValueType == ValueTypes::String, "Value type is different");

				return m_Data.String;
			}

			INLINE const WString &GetAsWString(void) const
			{
				Assert(m_ValueType == ValueTypes::WString, "Value type is different");

				return m_Data.WString;
			}

			INLINE const Vector2F &GetAsVector2F(void) const
			{
				Assert(m_ValueType == ValueTypes::Vector2F, "Value type is different");

				return m_Data.Vector2F;
			}

			INLINE const Vector3F &GetAsVector3F(void) const
			{
				Assert(m_ValueType == ValueTypes::Vector2F, "Value type is different");

				return m_Data.Vector3F;
			}

			INLINE const Matrix4F &GetAsMatrix4F(void) const
			{
				Assert(m_ValueType == ValueTypes::Matrix4F, "Value type is different");

				return m_Data.Matrix4F;
			}

			INLINE ValueTypes GetValueTypes(void) const
			{
				return m_ValueType;
			}

		private:
			Data m_Data;
			ValueTypes m_ValueType;
		};
	}
}
#endif
