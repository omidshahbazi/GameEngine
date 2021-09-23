// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef CACHE_POOL_H
#define CACHE_POOL_H

#include <EntityComponentSystem\Private\EntityCache.h>
#include <EntityComponentSystem\Private\ComponentCache.h>
#include <Allocators\AllocatorBase.h>
#include <Containers\Map.h>

namespace Engine
{
	using namespace Allocators;
	using namespace Containers;

	namespace EntityComponentSystem
	{
		namespace Private
		{
			class CachePool
			{
			public:
				CachePool(AllocatorBase* Allocator) :
					m_Allocator(Allocator),
					m_EntityCache(Allocator),
					m_ComponentCacheMap(Allocator)
				{
				}

				EntityCache& GetEntityCache(void)
				{
					return m_EntityCache;
				}

				const EntityCache& GetEntityCache(void) const
				{
					return m_EntityCache;
				}

				template<typename ComponentType>
				ComponentCache<ComponentType>& GetComponentCache(void)
				{
					uint32 typeID = ComponentTypeTraits::GetTypeID<ComponentType>();

					if (m_ComponentCacheMap.Contains(typeID))
						return *ReinterpretCast(ComponentCache<ComponentType>*, m_ComponentCacheMap[typeID]);

					ComponentCache<ComponentType>* cache = ReinterpretCast(ComponentCache<ComponentType>*, AllocateMemory(m_Allocator, sizeof(ComponentCache<ComponentType>)));
					Construct(cache, m_Allocator);

					m_ComponentCacheMap[typeID] = cache;

					return *cache;
				}

				template<typename ComponentType>
				const ComponentCache<ComponentType>& GetComponentCache(void) const
				{
					uint32 typeID = ComponentTypeTraits::GetTypeID<ComponentType>();

					if (m_ComponentCacheMap.Contains(typeID))
						return *ReinterpretCast(ComponentCache<ComponentType>*, m_ComponentCacheMap[typeID]);

					CoreDebugAssert(Categories::EntityComponentSystem, false, "Couldn't find a ComponentCache");
				}

			private:
				AllocatorBase* m_Allocator;
				EntityCache m_EntityCache;
				Map<uint32, void*> m_ComponentCacheMap;
			};
		}
	}
}

#endif