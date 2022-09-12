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
				if (Access == AccessSpecifiers::Public)
					m_PublicParentsName.Add(Value);
				else
					m_NonPublicParentsName.Add(Value);
			}

			void ImplementObjectType::AddNestedType(Type* Value, AccessSpecifiers Access)
			{
				if (Access == AccessSpecifiers::Public)
					m_PublicNestedTypes.Add(Value);
				else
					m_NonPublicNestedTypes.Add(Value);
			}

			void ImplementObjectType::AddFunction(FunctionType* Value, AccessSpecifiers Access)
			{
				if (Access == AccessSpecifiers::Public)
					m_PublicFunctions.Add(Value);
				else
					m_NonPublicFunctions.Add(Value);
			}

			void ImplementObjectType::AddProperty(PropertyType* Value, AccessSpecifiers Access)
			{
				if (Access == AccessSpecifiers::Public)
					m_PublicProperties.Add(Value);
				else
					m_NonPublicProperties.Add(Value);
			}
		}
	}
}