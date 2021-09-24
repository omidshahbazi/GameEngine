// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef REGISTRY_H
#define REGISTRY_H

#include <EntityComponentSystem\Entity.h>
#include <EntityComponentSystem\View.h>
#include <EntityComponentSystem\CommonTypes.h>
#include <EntityComponentSystem\Private\CachePool.h>

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

			template<typename ComponentType, typename... ParameterTypes>
			auto& AddComponent(const Entity& Entity, ParameterTypes&& ...Arguments)
			{
				static auto& cache = m_CachePool.GetComponentCache<ComponentType>();

				return cache.Create(Entity, std::forward<ParameterTypes>(Arguments)...);
			}

			template<typename ComponentType>
			void Destroy(const Entity& Entity)
			{
				static auto& cache = m_CachePool.GetComponentCache<ComponentType>();

				cache.Destroy(Entity);
			}

			template<typename ComponentType>
			bool HasComponent(const Entity& Entity) const
			{
				static const auto& cache = m_CachePool.GetComponentCache<ComponentType>();

				return cache.Has(Entity);
			}

			template<typename ComponentType>
			auto& GetComponent(const Entity& Entity)
			{
				static auto& cache = m_CachePool.GetComponentCache<ComponentType>();

				return cache.Get(Entity);
			}

			template<typename ComponentType>
			auto& GetComponent(const Entity& Entity) const
			{
				static const auto& cache = m_CachePool.GetComponentCache<ComponentType>();

				return cache.Get(Entity);
			}

			template<typename ComponentType>
			auto& GetOrAddComponent(const Entity& Entity)
			{
				if (HasComponent<ComponentType>(Entity))
					return GetComponent<ComponentType>(Entity);

				return AddComponent<ComponentType>(Entity);
			}

			template<typename... ComponentTypes, typename... ExcludeComponentTypes>
			auto GetView(ExcludeComponentTypeList<ExcludeComponentTypes...> Excludes = {}) const
			{
				View<ComponentTypeList<ComponentTypes...>, ExcludeComponentTypeList<ExcludeComponentTypes...>> view(m_Allocator, &m_CachePool);

				for (auto& entity : m_CachePool.GetEntityCache())
				{
					if (!(m_CachePool.GetComponentCache<ComponentTypes>().Has(entity) && ...))
						continue;

					if ((m_CachePool.GetComponentCache<ExcludeComponentTypes>().Has(entity) || ...))
						continue;

					view.Add(entity);
				}

				return view;
			}

		private:
			AllocatorBase* m_Allocator;
			Private::CachePool m_CachePool;
		};
	}
}

#endif