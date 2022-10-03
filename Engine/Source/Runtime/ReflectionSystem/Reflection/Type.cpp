// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\Type.h>
#include <Reflection\ObjectType.h>

namespace Engine
{
	namespace Reflection
	{
		Type::Type(ObjectType* TopNest) :
			m_TypeID((uint32)this),
			m_TopNest(TopNest)
		{
		}

		String Type::GetFullQualifiedName(void) const
		{
			return (m_TopNest == nullptr ? String::Empty : m_TopNest->GetFullQualifiedName() + "::") + m_Name;
		}
	}
}