// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef IMPLEMENT_OBJECT_TYPE_H
#define IMPLEMENT_OBJECT_TYPE_H
#include <Reflection\ObjectType.h>

namespace Engine
{
	namespace Reflection
	{
		namespace Private
		{
			class REFLECTION_API ImplementObjectType : public ObjectType
			{
			public:
				ImplementObjectType(AllocatorBase* Allocator, ObjectType* TopNest);

				virtual ~ImplementObjectType(void)
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

				INLINE void SetIsStruct(bool Value)
				{
					m_IsStruct = Value;
				}

				void AddParentName(const String& Value, AccessSpecifiers Access);

				void AddNestedType(Type* Value, AccessSpecifiers Access);

				void AddFunction(FunctionType* Value, AccessSpecifiers Access);

				void AddProperty(PropertyType* Value, AccessSpecifiers Access);
			};
		}
	}
}
#endif
