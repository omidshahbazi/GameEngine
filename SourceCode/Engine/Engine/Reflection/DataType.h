// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef DATA_TYPE_H
#define DATA_TYPE_H
#include <Common\ValueTypes.h>

namespace Engine
{
	namespace Reflection
	{
		class REFLECTION_API DataType
		{
		public:
			enum class PassesTypes
			{
				Value = 0,
				Reference,
				Pointer
			};

		public:
			DataType(void) :
				m_ValueType(ValueTypes::None),
				m_PassType(PassesTypes::Value),
				m_IsConst(false),
				m_IsConstValue(false)
			{
			}
			virtual ~DataType(void)
			{
			}

			INLINE ValueTypes GetValueType(void) const
			{
				return m_ValueType;
			}
			INLINE void SetValueType(ValueTypes Value)
			{
				m_ValueType = Value;
			}

			INLINE PassesTypes GetPassType(void) const
			{
				return m_PassType;
			}
			INLINE void SetPassType(PassesTypes Value)
			{
				m_PassType = Value;
			}

			INLINE const String &GetExtraValueType(void) const
			{
				return m_ExtraValueType;
			}
			INLINE void SetExtraValueType(const String &Value)
			{
				m_ExtraValueType = Value;
			}

			INLINE bool GetIsConst(void) const
			{
				return m_IsConst;
			}
			INLINE void SetIsConst(bool Value)
			{
				m_IsConst = Value;
			}

			INLINE bool GetIsConstValue(void) const
			{
				return m_IsConstValue;
			}
			INLINE void SetIsConstValue(bool Value)
			{
				m_IsConstValue = Value;
			}

			INLINE static String GetBasicDataTypeText(ValueTypes Type)
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

				//case ValueTypes::Vector2:
				//	return  "Vector2";
				//case ValueTypes::Vector3:
				//	return  "Vector3";
				//case ValueTypes::Matrix4:
				//	return  "Matrix4";
				}

				return "";
			}

			INLINE void GetSignature(String &Signature) const
			{
				if (m_IsConst)
					Signature += "const ";

				Signature += (m_ValueType == ValueTypes::None ? m_ExtraValueType : GetBasicDataTypeText(m_ValueType)) + " ";

				if (m_IsConstValue)
					Signature += "const ";

				if (m_PassType == PassesTypes::Reference)
					Signature += "& ";
				else if (m_PassType == PassesTypes::Pointer)
					Signature += "* ";
			}

		protected:
			ValueTypes m_ValueType;
			PassesTypes m_PassType;
			String m_ExtraValueType;
			bool m_IsConst;
			bool m_IsConstValue;
		};
	}
}
#endif
