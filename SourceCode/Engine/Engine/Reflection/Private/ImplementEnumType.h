// Copyright 2012-2015 ?????????????. All Rights Reserved.
#ifndef IMPLEMENT_ENUMTYPE_H
#define IMPLEMENT_ENUMTYPE_H
#include <Reflection\EnumType.h>

namespace Engine
{
	namespace Reflection
	{
		namespace Private
		{
			class REFLECTION_API ImplementEnumType : public EnumType
			{
			public:
				ImplementEnumType(void);
				virtual ~ImplementEnumType(void)
				{
				}

				INLINE void SetName(const String &Value)
				{
					m_Name = Value;
				}

				INLINE void AddItem(const String &Value)
				{
					m_Items.Add(Item(0, Value));
				}

				INLINE void AddItem(int32 Value, const String &Name)
				{
					m_Items.Add(Item(Value, Name));
				}
			};
		}
	}
}
#endif
