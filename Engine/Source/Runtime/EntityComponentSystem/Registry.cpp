// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EntityComponentSystem\Registry.h>

namespace Engine
{
	namespace EntityComponentSystem
	{
		Registry::Registry(AllocatorBase* Allocator) :
			m_Allocator(Allocator),
			m_CachePool(m_Allocator)
		{
			CoreDebugAssert(Categories::EntityComponentSystem, m_Allocator != nullptr, "Allocator cannot be null");
		}

		const Entity& Registry::Create(void)
		{
			return m_CachePool.GetEntityCache().Create();
		}

		void Registry::Destroy(const Entity& Entity)
		{
			m_CachePool.GetEntityCache().Destroy(Entity);
		}
	}
}