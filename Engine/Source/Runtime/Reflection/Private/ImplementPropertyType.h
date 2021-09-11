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
				ImplementPropertyType(Type *TopNest);
				virtual ~ImplementPropertyType(void)
				{
				}

				INLINE void SetName(const String &Value)
				{
					m_Name = Value;
				}

				INLINE void SetDataType(const DataType &Value)
				{
					m_DataType = Value;
				}

				INLINE uint32 GetOffset(void) const
				{
					return m_Offset;
				}
				INLINE void SetOffset(uint32 Value)
				{
					m_Offset = Value;
				}
			};
		}
	}
}
#endif
