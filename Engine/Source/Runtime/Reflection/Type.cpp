// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\Type.h>

namespace Engine
{
	namespace Reflection
	{
		Type::Type(Type *TopNest) :
			m_TypeID((uint32)this),
			m_TopNest(TopNest)
		{
		}
	}
}