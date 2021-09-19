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
			class EntityCache : private Cache<Entity>
			{
			public:
				EntityCache(AllocatorBase* Allocator) :
					Cache<Entity>(Allocator, 1024)
				{
				}

				Entity Create(void)
				{
					static uint32 lastID = 0;

					Entity& entity = *Allocate();

					entity = ++lastID;

					return entity;
				}

				void Destroy(Entity Entity)
				{

				}

				void Enable(Entity Entity)
				{

				}

				void Disable(Entity Entity)
				{

				}
			};
		}
	}
}

#endif