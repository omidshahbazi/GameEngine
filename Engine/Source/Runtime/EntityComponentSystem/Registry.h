// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef REGISTRY_H
#define REGISTRY_H

#include <EntityComponentSystem\Entity.h>
#include <EntityComponentSystem\View.h>
#include <EntityComponentSystem\Private\EntityCache.h>
#include <EntityComponentSystem\Private\ComponentCache.h>
#include <tuple>

namespace Engine
{
	using namespace Allocators;

	namespace EntityComponentSystem
	{
		class ENTITYCOMPONENTSYSTEM_API Registry
		{
		public:
			Registry(AllocatorBase* Allocator);

			const Entity& Create(void);
			void Destroy(const Entity& Entity);
			void Enable(const Entity& Entity);
			void Disable(const Entity& Entity);

			template<typename ComponentType, typename... ParameterTypes>
			auto AddComponent(const Entity& Entity, ParameterTypes&& ...Arguments)
			{
				static auto& cache = GetInstance<ComponentType>();

				return cache.Create(Entity, std::forward<ParameterTypes>(Arguments)...);
			}

			template<typename ComponentType>
			void Destroy(const Entity& Entity)
			{
				static auto& cache = GetInstance<ComponentType>();

				cache.Destroy(Entity);
			}

			template<typename ComponentType>
			void Enable(const Entity& Entity)
			{
				static auto& cache = GetInstance<ComponentType>();

				cache.Destroy(Entity);
			}

			template<typename ComponentType>
			void Disable(const Entity& Entity)
			{
				static auto& cache = GetInstance<ComponentType>();

				cache.Destroy(Entity);
			}

			template<typename ComponentType>
			auto HasComponent(const Entity& Entity)
			{
				static const auto& cache = GetInstance<ComponentType>();

				return cache.Has(Entity);
			}

			template<typename ComponentType>
			auto GetComponent(const Entity& Entity)
			{
				static auto& cache = GetInstance<ComponentType>();

				return cache.Get(Entity);
			}

			template<typename ComponentType>
			auto GetComponent(const Entity& Entity) const
			{
				static const auto& cache = GetInstance<ComponentType>();

				return cache.Get(Entity);
			}

			template<typename ComponentType>
			auto GetOrAddComponent(const Entity& Entity)
			{
				static auto& cache = GetInstance<ComponentType>();

				if (HasComponent<ComponentType>(Entity))
					return GetComponent<ComponentType>(Entity);

				return AddComponent<ComponentType>(Entity);
			}

			//template<typename... ComponentTypes>
			//View<ComponentTypes...> GetView(const Entity& Entity)
			//{
			//	static auto& cache = (GetInstance<ComponentType>(), ...);

			//	return View<ComponentTypes...>(cache);
			//}

			template<typename ComponentType>
			Private::ComponentCache<ComponentType>& GetInstance(void)
			{
				uint32 typeID = Private::ComponentTypeTraits::GetTypeID<ComponentType>();

				if (m_ComponentCacheMap.Contains(typeID))
					return *ReinterpretCast(Private::ComponentCache<ComponentType>*, m_ComponentCacheMap[typeID]);

				Private::ComponentCache<ComponentType>* cache = ReinterpretCast(Private::ComponentCache<ComponentType>*, AllocateMemory(m_Allocator, sizeof(Private::ComponentCache<ComponentType>)));
				Construct(cache, m_Allocator);

				m_ComponentCacheMap[typeID] = cache;

				return *cache;
			}

		private:
			AllocatorBase* m_Allocator;
			Private::EntityCache m_EntityCache;
			Map<uint32, void*> m_ComponentCacheMap;
		};
	}
}

#endif