// Copyright 2016-2020 ?????????????. All Rights Reserved.
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
				ImplementEnumType(ObjectType* TopNest);

				virtual ~ImplementEnumType(void)
				{
				}

				INLINE void SetName(const String& Value)
				{
					m_Name = Value;
				}

				INLINE void SetNamespace(const String& Namespace)
				{
					m_Namespace = Namespace;
				}

				INLINE void AddItem(const String& Name, int32 Value)
				{
					m_Items.Add(Item(Name, Value));
				}
			};
		}
	}
}
#endif
