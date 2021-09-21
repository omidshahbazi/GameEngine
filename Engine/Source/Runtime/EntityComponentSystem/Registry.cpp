// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EntityComponentSystem\Registry.h>

namespace Engine
{
	namespace EntityComponentSystem
	{
		Registry::Registry(AllocatorBase* Allocator) :
			m_Allocator(Allocator),
			m_EntityCache(m_Allocator),
			m_ComponentCacheMap(m_Allocator)
		{
			CoreDebugAssert(Categories::EntityComponentSystem, m_Allocator != nullptr, "Allocator cannot be null");
		}

		const Entity& Registry::Create(void)
		{
			return m_EntityCache.Create();
		}

		void Registry::Destroy(const Entity& Entity)
		{
			m_EntityCache.Destroy(Entity);
		}

		void Registry::Enable(const Entity& Entity)
		{
			m_EntityCache.Enable(Entity);
		}

		void Registry::Disable(const Entity& Entity)
		{
			m_EntityCache.Disable(Entity);
		}
	}
}