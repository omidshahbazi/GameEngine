// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\Private\ImplementObjectType.h>

namespace Engine
{
	namespace Reflection
	{
		namespace Private
		{
			ImplementObjectType::ImplementObjectType(AllocatorBase* Allocator, ObjectType* TopNest) :
				ObjectType(Allocator, TopNest)
			{
			}

			void ImplementObjectType::AddParentName(const String& Value, AccessSpecifiers Access)
			{
				if (m_ParentNames.Contains(Access))
				{
					m_ParentNames[Access].Add(Value);
					return;
				}

				auto& list = m_ParentNames[Access] = StringList(m_ParentNames.GetAllocator());
				list.Add(Value);
			}

			void ImplementObjectType::AddNestedType(Type* Value, AccessSpecifiers Access)
			{
				if (m_NestedTypes.Contains(Access))
				{
					m_NestedTypes[Access].Add(Value);
					return;
				}

				auto& list = m_NestedTypes[Access] = TypeList(m_ParentNames.GetAllocator());
				list.Add(Value);
			}

			void ImplementObjectType::AddFunction(FunctionType* Value, AccessSpecifiers Access)
			{
				if (m_Functions.Contains(Access))
				{
					m_Functions[Access].Add(Value);
					return;
				}

				auto& list = m_Functions[Access] = FunctionTypeList(m_ParentNames.GetAllocator());
				list.Add(Value);
			}

			void ImplementObjectType::AddProperty(PropertyType* Value, AccessSpecifiers Access)
			{
				if (m_Properties.Contains(Access))
				{
					m_Properties[Access].Add(Value);
					return;
				}

				auto& list = m_Properties[Access] = PropertyTypeList(m_ParentNames.GetAllocator());
				list.Add(Value);
			}
		}
	}
}