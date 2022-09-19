// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\PropertyType.h>
#include <Reflection\Enum.h>
#include <Containers\Exception.h>

namespace Engine
{
	namespace Reflection
	{
		PropertyType::PropertyType(ObjectType* TopNest) :
			Type(TopNest),
			m_IsReadOnly(false)
		{
		}

		AnyDataType PropertyType::GetValue(void) const
		{
			return GetValue(nullptr);
		}

		void PropertyType::SetValue(const AnyDataType& Value)
		{
			return SetValue(nullptr, Value);
		}
	}
}