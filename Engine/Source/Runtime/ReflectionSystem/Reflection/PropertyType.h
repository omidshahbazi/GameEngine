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
		protected:
			PropertyType(ObjectType* TopNest);

		public:
			virtual ~PropertyType(void)
			{
			}

			AnyDataType GetValue(const void* TargetObject) const;
			void SetValue(void* TargetObject, const AnyDataType& Value);

			INLINE const DataType& GetDataType(void) const
			{
				return m_DataType;
			}

		protected:
			uint32 m_Offset;
			DataType m_DataType;
		};

		typedef Vector<PropertyType*> PropertyTypeList;
	}
}

#endif
