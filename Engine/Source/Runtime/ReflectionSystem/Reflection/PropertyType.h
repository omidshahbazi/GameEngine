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

			virtual AnyDataType GetValue(void) const;
			virtual void SetValue(const AnyDataType& Value);

			virtual AnyDataType GetValue(const void* TargetObject) const = 0;
			virtual void SetValue(void* TargetObject, const AnyDataType& Value) = 0;

			INLINE const DataType& GetDataType(void) const
			{
				return m_DataType;
			}

			INLINE bool GetIsReadOnly(void) const
			{
				return m_IsReadOnly;
			}

			INLINE bool GetIsStatic(void) const
			{
				return m_IsStatic;
			}

		protected:
			DataType m_DataType;
			bool m_IsReadOnly;
			bool m_IsStatic;
		};

		typedef Vector<PropertyType*> PropertyTypeList;
	}
}

#endif
