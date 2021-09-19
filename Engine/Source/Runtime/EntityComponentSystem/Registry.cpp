// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EntityComponentSystem\Registry.h>

namespace Engine
{
	namespace EntityComponentSystem
	{
		Registry::Registry(AllocatorBase* Allocator) :
			m_Allocator(Allocator),
			m_EntityCache(m_Allocator)
		{
		}

		Entity Registry::Create(void)
		{
			return m_EntityCache.Create();
		}

		void Registry::Destroy(Entity Entity)
		{
			m_EntityCache.Destroy(Entity);
		}
	}
}