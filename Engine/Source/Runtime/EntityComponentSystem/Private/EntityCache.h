// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef ENTITY_CACHE_H
#define ENTITY_CACHE_H

#include <EntityComponentSystem\Entity.h>
#include <EntityComponentSystem\Private\Cache.h>

namespace Engine
{
	namespace EntityComponentSystem
	{
		namespace Private
		{
			class EntityCache : public Cache<Entity>
			{
			private:
				typedef Cache<Entity> CacheType;

			public:
				EntityCache(AllocatorBase* Allocator) :
					CacheType(Allocator, 1024)
				{
				}

				const Entity& Create(void)
				{
					static uint32 lastID = 0;

					Entity& entity = *CacheType::Allocate();

					entity = ++lastID;

					return entity;
				}

				void Destroy(const Entity& Entity)
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Entity != Entity::Null, "Entity cannot be null");

					CacheType::Deallocate(GetAddress(Entity));
				}

				void Reorder(const Entity& Left, const Entity& Right)
				{
					Entity* left = GetAddress(Left);
					Entity* right = GetAddress(Right);

					Swap(left, right);
				}

			private:
				Entity* GetAddress(const Entity& EntityIdentifier)
				{
					Entity* result = CacheType::Find([&EntityIdentifier](Entity& item) { return (item == EntityIdentifier); });

					CoreDebugAssert(Categories::EntityComponentSystem, result != nullptr, "Couldn't find address of entity");

					return result;
				}
			};
		}
	}
}

#endif