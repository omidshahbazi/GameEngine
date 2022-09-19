// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef IMPLEMENT_PROPERTY_TYPE_H
#define IMPLEMENT_PROPERTY_TYPE_H
#include <Reflection\PropertyType.h>

namespace Engine
{
	namespace Reflection
	{
		namespace Private
		{
			class REFLECTION_API ImplementPropertyType : public PropertyType
			{
			public:
				ImplementPropertyType(ObjectType* TopNest);
				virtual ~ImplementPropertyType(void)
				{
				}

				INLINE void SetName(const String& Value)
				{
					m_Name = Value;
				}

				INLINE void SetDataType(const DataType& Value)
				{
					m_DataType = Value;
				}

				INLINE void SetIsReadOnly(bool Value)
				{
					m_IsReadOnly = Value;
				}

				INLINE void SetIsStatic(bool Value)
				{
					m_IsStatic = Value;
				}
			};
		}
	}
}
#endif
