// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef PROPERTY_TYPE_H
#define PROPERTY_TYPE_H
#include <Reflection\Type.h>
#include <Reflection\DataType.h>

namespace Engine
{
	namespace Reflection
	{
		class REFLECTION_API PropertyType : public Type
		{
		public:
			PropertyType(Type *TopNest) :
				Type(TopNest),
				m_Offset(0)
			{
			}
			PropertyType(Type *TopNest, ValueTypes ValueType) :
				Type(TopNest)
			{
				m_DataType.SetValueType(ValueType);
			}
			virtual ~PropertyType(void)
			{
			}

			INLINE Types GetType(void) const
			{
				return Types::Property;
			}

			INLINE const DataType &GetDataType(void) const
			{
				return m_DataType;
			}

		protected:
			uint32 m_Offset;
			DataType m_DataType;
		};
	}
}

#endif
