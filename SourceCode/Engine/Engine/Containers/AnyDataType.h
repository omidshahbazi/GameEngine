// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef ANY_DATA_TYPE_H
#define ANY_DATA_TYPE_H

#include <Common\PrimitiveTypes.h>
#include <Common\ValueTypes.h>
#include <Containers\Strings.h>
#include <Containers\Color.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	using namespace Common;
	using namespace MathContainers;

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
					PlatformMemory::Set(this, 0, 1);
				}

				~Data(void)
				{
				}

				void* VoidPointer;

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

				ColorUI8 ColorUI8;

				Vector2F Vector2F;
				Vector2I Vector2I;
				Vector3F Vector3F;
				Vector4F Vector4F;
				Matrix4F Matrix4F;
			};

		public:
			AnyDataType(void) :
				m_ValueType(ValueTypes::None)
			{
			}

			AnyDataType(const AnyDataType& Other) :
				m_ValueType(ValueTypes::None)
			{
				*this = Other;
			}

			AnyDataType(void* Value) :
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

			AnyDataType(const String& Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(const WString& Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(const ColorUI8& Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(const Vector2F& Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(const Vector2I& Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(const Vector3F& Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(const Vector4F& Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			AnyDataType(const Matrix4F& Value) :
				m_ValueType(ValueTypes::None)
			{
				*this = Value;
			}

			~AnyDataType(void)
			{
				if (m_ValueType == ValueTypes::String)
					Destruct(&m_Data.String);
				else if (m_ValueType == ValueTypes::WString)
					Destruct(&m_Data.WString);
			}

			template<typename T>
			INLINE T Get(void) const
			{
				return *GetPointer<T>();
			}

			template<typename T>
			INLINE T* GetPointer(void) const
			{
				return ReinterpretCast(T*, ReinterpretCast(void*, ConstCast(Data*, &m_Data)));
			}

			INLINE void* GetAsVoid(void) const
			{
				HardAssert(m_ValueType == ValueTypes::VoidPointer, "Value type is different");

				return m_Data.VoidPointer;
			}

			INLINE bool GetAsBool(void) const
			{
				HardAssert(m_ValueType == ValueTypes::Bool, "Value type is different");

				return m_Data.Bool;
			}

			INLINE int8 GetAsInt8(void) const
			{
				HardAssert(m_ValueType == ValueTypes::Int8, "Value type is different");

				return m_Data.Int8;
			}

			INLINE int16 GetAsInt16(void) const
			{
				HardAssert(m_ValueType == ValueTypes::Int16, "Value type is different");

				return m_Data.Int16;
			}

			INLINE int32 GetAsInt32(void) const
			{
				HardAssert(m_ValueType == ValueTypes::Int32, "Value type is different");

				return m_Data.Int32;
			}

			INLINE int64 GetAsInt64(void) const
			{
				HardAssert(m_ValueType == ValueTypes::Int64, "Value type is different");

				return m_Data.Int64;
			}

			INLINE uint8 GetAsUInt8(void) const
			{
				HardAssert(m_ValueType == ValueTypes::UInt8, "Value type is different");

				return m_Data.UInt8;
			}

			INLINE uint16 GetAsUInt16(void) const
			{
				HardAssert(m_ValueType == ValueTypes::UInt16, "Value type is different");

				return m_Data.UInt16;
			}

			INLINE uint32 GetAsUInt32(void) const
			{
				HardAssert(m_ValueType == ValueTypes::UInt32, "Value type is different");

				return m_Data.UInt32;
			}

			INLINE uint64 GetAsUInt64(void) const
			{
				HardAssert(m_ValueType == ValueTypes::UInt64, "Value type is different");

				return m_Data.UInt64;
			}

			INLINE float32 GetAsFloat32(void) const
			{
				HardAssert(m_ValueType == ValueTypes::Float32, "Value type is different");

				return m_Data.Float32;
			}

			INLINE float64 GetAsFloat64(void) const
			{
				HardAssert(m_ValueType == ValueTypes::Float64, "Value type is different");

				return m_Data.Float64;
			}

			INLINE const String& GetAsString(void) const
			{
				HardAssert(m_ValueType == ValueTypes::String, "Value type is different");

				return m_Data.String;
			}

			INLINE const WString& GetAsWString(void) const
			{
				HardAssert(m_ValueType == ValueTypes::WString, "Value type is different");

				return m_Data.WString;
			}

			INLINE const ColorUI8& GetAsColorUI8(void) const
			{
				HardAssert(m_ValueType == ValueTypes::ColorUI8, "Value type is different");

				return m_Data.ColorUI8;
			}

			INLINE const Vector2F& GetAsVector2F(void) const
			{
				HardAssert(m_ValueType == ValueTypes::Vector2F, "Value type is different");

				return m_Data.Vector2F;
			}

			INLINE const Vector2I& GetAsVector2I(void) const
			{
				HardAssert(m_ValueType == ValueTypes::Vector2I, "Value type is different");

				return m_Data.Vector2I;
			}

			INLINE const Vector3F& GetAsVector3F(void) const
			{
				HardAssert(m_ValueType == ValueTypes::Vector3F, "Value type is different");

				return m_Data.Vector3F;
			}

			INLINE const Vector4F& GetAsVector4F(void) const
			{
				HardAssert(m_ValueType == ValueTypes::Vector4F, "Value type is different");

				return m_Data.Vector4F;
			}

			INLINE const Matrix4F& GetAsMatrix4F(void) const
			{
				HardAssert(m_ValueType == ValueTypes::Matrix4F, "Value type is different");

				return m_Data.Matrix4F;
			}

			INLINE ValueTypes GetValueType(void) const
			{
				return m_ValueType;
			}

			INLINE uint8 GetDataSize(void) const
			{
				return GetDataSize(m_ValueType);
			}

			INLINE AnyDataType& operator= (const AnyDataType& Other)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == Other.m_ValueType, "New value should have same type as other's value");

				if (Other.m_ValueType == ValueTypes::String)
					m_Data.String = Other.m_Data.String;
				else if (Other.m_ValueType == ValueTypes::WString)
					m_Data.WString = Other.m_Data.WString;
				else
					PlatformMemory::Copy(&Other.m_Data, &m_Data, 1);

				m_ValueType = Other.m_ValueType;

				return *this;
			}

			INLINE AnyDataType& operator= (void* Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::VoidPointer, "Value types are mismatched");

				m_Data.VoidPointer = Value;
				m_ValueType = ValueTypes::VoidPointer;

				return *this;
			}

			INLINE AnyDataType& operator= (bool Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Bool, "Value types are mismatched");

				m_Data.Bool = Value;
				m_ValueType = ValueTypes::Bool;

				return *this;
			}

			INLINE AnyDataType& operator= (int8 Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Int8, "Value types are mismatched");

				m_Data.Int8 = Value;
				m_ValueType = ValueTypes::Int8;

				return *this;
			}

			INLINE AnyDataType& operator= (int16 Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Int16, "Value types are mismatched");

				m_Data.Int16 = Value;
				m_ValueType = ValueTypes::Int16;

				return *this;
			}

			INLINE AnyDataType& operator= (int32 Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Int32, "Value types are mismatched");

				m_Data.Int32 = Value;
				m_ValueType = ValueTypes::Int32;

				return *this;
			}

			INLINE AnyDataType& operator= (int64 Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Int64, "Value types are mismatched");

				m_Data.Int64 = Value;
				m_ValueType = ValueTypes::Int64;

				return *this;
			}

			INLINE AnyDataType& operator= (uint8 Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::UInt8, "Value types are mismatched");

				m_Data.UInt8 = Value;
				m_ValueType = ValueTypes::UInt8;

				return *this;
			}

			INLINE AnyDataType& operator= (uint16 Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::UInt16, "Value types are mismatched");

				m_Data.UInt16 = Value;
				m_ValueType = ValueTypes::UInt16;

				return *this;
			}

			INLINE AnyDataType& operator= (uint32 Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::UInt32, "Value types are mismatched");

				m_Data.UInt32 = Value;
				m_ValueType = ValueTypes::UInt32;

				return *this;
			}

			INLINE AnyDataType& operator= (uint64 Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::UInt64, "Value types are mismatched");

				m_Data.UInt64 = Value;
				m_ValueType = ValueTypes::UInt64;

				return *this;
			}

			INLINE AnyDataType& operator= (float32 Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Float32, "Value types are mismatched");

				m_Data.Float32 = Value;
				m_ValueType = ValueTypes::Float32;

				return *this;
			}

			INLINE AnyDataType& operator= (float64 Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Float64, "Value types are mismatched");

				m_Data.Float64 = Value;
				m_ValueType = ValueTypes::Float64;

				return *this;
			}

			INLINE AnyDataType& operator= (const String& Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::String, "Value types are mismatched");

				m_Data.String = Value;
				m_ValueType = ValueTypes::String;

				return *this;
			}

			INLINE AnyDataType& operator= (const WString& Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::WString, "Value types are mismatched");

				m_Data.WString = Value;
				m_ValueType = ValueTypes::WString;

				return *this;
			}

			INLINE AnyDataType& operator= (const ColorUI8& Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::ColorUI8, "Value types are mismatched");

				m_Data.ColorUI8 = Value;
				m_ValueType = ValueTypes::ColorUI8;

				return *this;
			}

			INLINE AnyDataType& operator= (const Vector2F& Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Vector2F, "Value types are mismatched");

				m_Data.Vector2F = Value;
				m_ValueType = ValueTypes::Vector2F;

				return *this;
			}

			INLINE AnyDataType& operator= (const Vector2I& Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Vector2I, "Value types are mismatched");

				m_Data.Vector2I = Value;
				m_ValueType = ValueTypes::Vector2I;

				return *this;
			}

			INLINE AnyDataType& operator= (const Vector3F& Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Vector3F, "Value types are mismatched");

				m_Data.Vector3F = Value;
				m_ValueType = ValueTypes::Vector3F;

				return *this;
			}

			INLINE AnyDataType& operator= (const Vector4F& Value)
			{
				HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Vector4F, "Value types are mismatched");

				m_Data.Vector4F = Value;
				m_ValueType = ValueTypes::Vector4F;

				return *this;
			}

			INLINE AnyDataType& operator= (const Matrix4F& Value)
			{
				//HardAssert(m_ValueType == ValueTypes::None || m_ValueType == ValueTypes::Matrix4F, "Value types are mismatched");

				m_Data.Matrix4F = Value;
				m_ValueType = ValueTypes::Matrix4F;

				return *this;
			}

			INLINE static uint8 GetDataSize(ValueTypes Type)
			{
				switch (Type)
				{
				case ValueTypes::None:
					return 0;
				case ValueTypes::VoidPointer:
					return sizeof(void*);
				case ValueTypes::Bool:
					return sizeof(bool);
				case ValueTypes::Int8:
					return sizeof(int8);
				case ValueTypes::Int16:
					return sizeof(int16);
				case ValueTypes::Int32:
					return sizeof(int32);
				case ValueTypes::Int64:
					return sizeof(int64);
				case ValueTypes::UInt8:
					return sizeof(uint8);
				case ValueTypes::UInt16:
					return sizeof(uint16);
				case ValueTypes::UInt32:
					return sizeof(uint32);
				case ValueTypes::UInt64:
					return sizeof(uint64);
				case ValueTypes::Float32:
					return sizeof(float32);
				case ValueTypes::Float64:
					return sizeof(float64);
				case ValueTypes::String:
					return sizeof(String);
				case ValueTypes::WString:
					return sizeof(WString);
				case ValueTypes::ColorUI8:
					return sizeof(ColorUI8);
				case ValueTypes::Vector2F:
					return sizeof(Vector2F);
				case ValueTypes::Vector2I:
					return sizeof(Vector2I);
				case ValueTypes::Vector3F:
					return sizeof(Vector3F);
				case ValueTypes::Vector4F:
					return sizeof(Vector3F);
				case ValueTypes::Matrix4F:
					return sizeof(Matrix4F);
				}

				return 0;
			}

			INLINE static bool IsNumericValueType(ValueTypes Type)
			{
				switch (Type)
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
					return true;
				}

				return false;
			}

			INLINE static String GetValueTypeText(ValueTypes Type)
			{
				switch (Type)
				{
				case ValueTypes::VoidPointer:
					return "void";

				case ValueTypes::Bool:
					return "bool";

				case ValueTypes::UInt8:
					return  "uint8";
				case ValueTypes::UInt16:
					return  "uint16";
				case ValueTypes::UInt32:
					return  "uint32";
				case ValueTypes::UInt64:
					return  "uint64";

				case ValueTypes::Int8:
					return "int8";
				case ValueTypes::Int16:
					return "int16";
				case ValueTypes::Int32:
					return "int32";
				case ValueTypes::Int64:
					return  "int64";

				case ValueTypes::Float32:
					return "float32";
				case ValueTypes::Float64:
					return "float64";

				case ValueTypes::String:
					return "String";
				case ValueTypes::WString:
					return "WString";

				case ValueTypes::Vector2F:
					return  "Vector2F";
				case ValueTypes::Vector3F:
					return  "Vector3F";
				case ValueTypes::Matrix4F:
					return  "Matrix4F";
				}

				return "";
			}

		private:
			Data m_Data;
			ValueTypes m_ValueType;
		};
	}
}
#endif
