// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef IMPLEMENT_DATA_STRUCTURE_TYPE_H
#define IMPLEMENT_DATA_STRUCTURE_TYPE_H
#include <Reflection\DataStructureType.h>

namespace Engine
{
	namespace Reflection
	{
		namespace Private
		{
			class REFLECTION_API ImplementDataStructureType : public DataStructureType
			{
			public:
				ImplementDataStructureType(AllocatorBase* Allocator, DataStructureType* TopNest);

				virtual ~ImplementDataStructureType(void)
				{
				}

				void AddParentName(const String& Value, AccessSpecifiers Access);

				void AddNestedType(Type* Value, AccessSpecifiers Access);

				void AddFunction(Type* Value, AccessSpecifiers Access);

				void AddProperty(Type* Value, AccessSpecifiers Access);

				INLINE void SetName(const String& Value)
				{
					m_Name = Value;
				}
			};
		}
	}
}
#endif
