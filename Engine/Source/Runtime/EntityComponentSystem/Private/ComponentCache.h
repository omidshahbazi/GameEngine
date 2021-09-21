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
					InfoType* infoType = GetAddress(Entity);
					Destruct(&infoType->Value);

					CacheType::Deallocate(infoType);
				}

				void Enable(const Entity& Entity)
				{
					CacheType::Enable(GetAddress(Entity));
				}

				void Disable(const Entity& Entity)
				{
					CacheType::Disable(GetAddress(Entity));
				}

				bool Has(const Entity& Entity) const
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Entity != Entity::Null, "Entity cannot be null");

					bool result = false;

					CacheType::ForEach([&Entity, &result](const InfoType& item)
						{
							if (item.BelongsTo != Entity)
								return true;

							result = true;
							return false;
						}, true);

					return result;
				}

				ComponentType* Get(const Entity& Entity)
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Entity != Entity::Null, "Entity cannot be null");

					ComponentType* result = nullptr;

					CacheType::ForEach([&Entity, &result](InfoType& item)
						{
							if (item.BelongsTo != Entity)
								return true;

							result = &item.Value;
							return false;
						}, true);

					return result;
				}

				ComponentType* Get(const Entity& Entity) const
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Entity != Entity::Null, "Entity cannot be null");

					const ComponentType* result = nullptr;

					CacheType::ForEach([&Entity, &result](const InfoType& item)
						{
							if (item.BelongsTo != Entity)
								return true;

							result = &item.Value;
							return false;
						}, true);

					return result;
				}

			private:
				InfoType* GetAddress(const Entity& Entity)
				{
					InfoType* result = nullptr;

					CacheType::ForEach([&Entity, &result](InfoType& item)
						{
							if (item.BelongsTo != Entity)
								return true;

							result = &item;
							return false;
						}, true);

					CoreDebugAssert(Categories::EntityComponentSystem, result != nullptr, "Couldn't find address of entity");

					return result;
				}
			};
		}
	}
}

#endif