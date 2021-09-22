// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMPONENT_HASH_H
#define COMPONENT_HASH_H

#include <EntityComponentSystem\Entity.h>
#include <EntityComponentSystem\Private\Cache.h>
#include <EntityComponentSystem\Private\ComponentTypeTraits.h>
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
			template<typename ComponentType>
			struct ComponentInfo
			{
			public:
				Entity BelongsTo;
				ComponentType Value;
			};

			template<typename ComponentType>
			class ComponentCache : private Cache<ComponentInfo<ComponentType>>
			{
			private:
				typedef ComponentInfo<ComponentType> InfoType;
				typedef Cache<InfoType> CacheType;

			public:
				ComponentCache(AllocatorBase* Allocator) :
					CacheType(Allocator, 1024)
				{
				}

				template<typename... ParameterTypes>
				ComponentType* Create(const Entity& Entity, ParameterTypes&& ...Arguments)
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Entity != Entity::Null, "Entity cannot be null");

					InfoType* info = CacheType::Allocate();

					info->BelongsTo = Entity;

					Construct(&info->Value, std::forward<ParameterTypes>(Arguments)...);

					return &info->Value;
				}

				void Destroy(const Entity& Entity)
				{
					InfoType* info = GetAddress(Entity);
					Destruct(&info->Value);

					CacheType::Deallocate(info);
				}

				bool Has(const Entity& Entity) const
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Entity != Entity::Null, "Entity cannot be null");

					return (Get(Entity) != nullptr);
				}

				ComponentType* Get(const Entity& Entity)
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Entity != Entity::Null, "Entity cannot be null");

					InfoType* info = FindInfo(Entity);
					if (info == nullptr)
						return nullptr;

					return &info->Value;
				}

				const ComponentType* Get(const Entity& Entity) const
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Entity != Entity::Null, "Entity cannot be null");

					const InfoType* info = CacheType::Find([&Entity](const InfoType& item) { return (item.BelongsTo == Entity); });
					if (info == nullptr)
						return nullptr;

					return &info->Value;
				}

			private:
				InfoType* FindInfo(const Entity& Entity)
				{
					return CacheType::Find([&Entity](InfoType& item) { return (item.BelongsTo == Entity); });
				}

				InfoType* GetAddress(const Entity& Entity)
				{
					InfoType* result = FindInfo(Entity);

					CoreDebugAssert(Categories::EntityComponentSystem, result != nullptr, "Couldn't find address of entity");

					return result;
				}
			};
		}
	}
}

#endif