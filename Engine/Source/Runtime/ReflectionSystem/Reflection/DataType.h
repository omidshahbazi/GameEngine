// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef DATA_TYPE_H
#define DATA_TYPE_H
#include <Common\ValueTypes.h>
#include <Common\PrimitiveTypes.h>
#include <Containers\AnyDataType.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Reflection
	{
		class PropertyType;
		class FunctionType;
		class ParameterType;

		class REFLECTION_API DataType
		{
			friend class PropertyType;
			friend class FunctionType;
			friend class ParameterType;

		public:
			enum class PassesTypes
			{
				Value = 0,
				Reference,
				Pointer
			};

		protected:
			DataType(void) :
				m_ValueType(ValueTypes::None),
				m_PassType(PassesTypes::Value),
				m_IsConst(false),
				m_IsConstValue(false)
			{
			}

		public:
			virtual ~DataType(void)
			{
			}

			INLINE ValueTypes GetValueType(void) const
			{
				return m_ValueType;
			}

			INLINE PassesTypes GetPassType(void) const
			{
				return m_PassType;
			}

			INLINE const String& GetExtraValueType(void) const
			{
				return m_ExtraValueType;
			}

			INLINE bool GetIsConst(void) const
			{
				return m_IsConst;
			}

			INLINE bool GetIsConstValue(void) const
			{
				return m_IsConstValue;
			}

			INLINE uint8 GetDataSize(void) const
			{
				if (m_PassType == PassesTypes::Value)
					return AnyDataType::GetDataSize(m_ValueType);

				return sizeof(void*);
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
