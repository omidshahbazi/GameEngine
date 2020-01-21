// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\Type.h>

namespace Engine
{
	namespace Reflection
	{
		Type::Type(const String &Name) :
			m_TypeID((uint32)this),
			m_Name(Name),
			m_TopNest(nullptr)
		{
		}


		Type::Type(Type *TopNest) :
			m_TypeID((uint32)this),
			m_TopNest(TopNest)
		{
		}


		Type::Type(const String &Name, Type *TopNest) :
			m_TypeID((uint32)this),
			m_Name(Name),
			m_TopNest(TopNest)
		{
		}
	}
}