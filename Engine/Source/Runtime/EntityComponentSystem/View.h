// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef VIEW_H
#define VIEW_H

#include <EntityComponentSystem\Entity.h>
#include <EntityComponentSystem\Private\Iterator.h>
#include <EntityComponentSystem\Private\CachePool.h>
#include <Allocators\AllocatorBase.h>
#include <Containers\Vector.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Debugging;
	using namespace Allocators;
	using namespace Containers;

	namespace EntityComponentSystem
	{
		class Registry;

		template<typename ComponentTypesList, typename ExcludeComponentTypesList>
		class View
		{
			friend class Registry;

		public:
			typedef Vector<Entity> ListType;

		private:
			View(AllocatorBase* Allocator, const Private::CachePool* CachePool) :
				m_Entities(Allocator),
				m_CachePool(CachePool)
			{
			}

			View(View&& Other) :
				m_Entities(std::move(Other.m_Entities))
			{
			}

			void Add(const Entity& Entity)
			{
				m_Entities.Add(Entity);
			}

		public:
			//template<typename ComponentType>
			//void Sort(std::function<bool(const ComponentType&, const ComponentType&)> IsLessThan)
			//{
			//	CoreDebugAssert(Categories::EntityComponentSystem, IsLessThan != nullptr, "IsLessThan cannot be null");

			//	Sort<ComponentType>([&](const Entity& LeftEntity, const ComponentType& LeftComponent, const Entity& RightEntity, const ComponentType& RightComponent)
			//		{
			//			return IsLessThan(LeftComponent, RightComponent);
			//		});
			//}

			//template<typename ComponentType>
			//void Sort(std::function<bool(const Entity&, const ComponentType&, const Entity&, const ComponentType&)> IsLessThan)
			//{
			//	CoreDebugAssert(Categories::EntityComponentSystem, IsLessThan != nullptr, "IsLessThan cannot be null");

			//	static auto& cache = m_Caches.get<Private::ComponentCache<ComponentType>>();

			//	cache.Sort([&](const Entity& LeftEntity, const ComponentType& LeftComponent, const Entity& RightEntity, const ComponentType& RightComponent)
			//		{
			//			bool result = IsLessThan(LeftEntity, LeftComponent, RightEntity, RightComponent);

			//			if (result)
			//				m_EntityCache.Reorder(LeftEntity, RightEntity);

			//			return result;
			//		});
			//}

			//Filter

			//	void Each

			//	Size()
			//	Contains()

			ListType::Iterator begin(void)
			{
				return m_Entities.GetBegin();
			}

			ListType::Iterator end(void)
			{
				return m_Entities.GetEnd();
			}

			uint32 GetSize(void) const
			{
				return m_Entities.GetSize();
			}

		private:
			ListType m_Entities;
			const Private::CachePool* m_CachePool;
		};
	}
}

#endif