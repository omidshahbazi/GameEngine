// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef REGISTRY_H
#define REGISTRY_H

#include <EntityComponentSystem\Entity.h>
#include <EntityComponentSystem\Private\EntityCache.h>
#include <EntityComponentSystem\Private\ComponentCache.h>

namespace Engine
{
	namespace EntityComponentSystem
	{
		using namespace Private;

		class ENTITYCOMPONENTSYSTEM_API Registry
		{
		public:
			Registry(AllocatorBase* Allocator);

			Entity Create(void);
			void Destroy(Entity Entity);

			template<typename ComponentType, typename... ParameterTypes>
			auto AddComponent(Entity Entity, ParameterTypes&& ...Arguments)
			{
				static ComponentCache<ComponentType> cache(m_Allocator);

				return cache.Create(Entity, std::forward<ParameterTypes>(Arguments)...);
			}

			//template<typename... ComponentType>
			//auto AddComponent(Entity Entity)
			//{

			//}

			template<typename... ComponentType>
			auto GetOrAddComponent(Entity Entity)
			{

			}

			template<typename... ComponentType>
			auto GetComponent(Entity Entity)
			{

			}

		private:
			AllocatorBase* m_Allocator;
			Private::EntityCache m_EntityCache;
		};
	}
}

#endif