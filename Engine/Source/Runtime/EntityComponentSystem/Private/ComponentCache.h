// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMPONENT_HASH_H
#define COMPONENT_HASH_H

#include <EntityComponentSystem\Entity.h>
#include <EntityComponentSystem\Private\Cache.h>
#include <EntityComponentSystem\Private\ComponentTypeTraits.h>
#include <Allocators\AllocatorBase.h>

namespace Engine
{
	using namespace Allocators;

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
				ComponentType& Create(const Entity& Entity, ParameterTypes&& ...Arguments)
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Entity != Entity::Null, "Entity cannot be null");

					auto info = CacheType::Allocate();

					info->BelongsTo = Entity;

					Construct(&info->Value, std::forward<ParameterTypes>(Arguments)...);

					return info->Value;
				}

				void Destroy(const Entity& Entity)
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Entity != Entity::Null, "Entity cannot be null");

					auto info = GetAddress(Entity);
					Destruct(&info->Value);

					CacheType::Deallocate(ConstCast(InfoType*, info));
				}

				bool Has(const Entity& Entity) const
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Entity != Entity::Null, "Entity cannot be null");

					auto info = FindAddress(Entity);

					return (info != nullptr);
				}

				ComponentType& Get(const Entity& Entity)
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Entity != Entity::Null, "Entity cannot be null");

					auto info = GetAddress(Entity);

					return info->Value;
				}

				const ComponentType& Get(const Entity& Entity) const
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Entity != Entity::Null, "Entity cannot be null");

					auto info = GetAddress(Entity);

					return info->Value;
				}

				void Sort(std::function<bool(const Entity&, const ComponentType&, const Entity&, const ComponentType&)> IsLessThan)
				{
					CoreDebugAssert(Categories::EntityComponentSystem, IsLessThan != nullptr, "IsLessThan cannot be null");

					CacheType::Sort([&IsLessThan](const InfoType& Left, const InfoType& Right)
						{
							return IsLessThan(Left.BelongsTo, Left.Value, Right.BelongsTo, Right.Value);
						});
				}

			private:
				InfoType* FindAddress(const Entity& Entity)
				{
					return CacheType::Find([&Entity](auto& item) { return (item.BelongsTo == Entity); });
				}

				const InfoType* FindAddress(const Entity& Entity) const
				{
					return CacheType::Find([&Entity](auto& item) { return (item.BelongsTo == Entity); });
				}

				InfoType* GetAddress(const Entity& Entity)
				{
					InfoType* result = FindAddress(Entity);

					THROW_IF_ENTITY_COMPONENT_SYSTEM_EXCEPTION(result != nullptr, "Entity doesn't have this component");

					return result;
				}

				const InfoType* GetAddress(const Entity& Entity) const
				{
					const InfoType* result = FindAddress(Entity);

					THROW_IF_ENTITY_COMPONENT_SYSTEM_EXCEPTION(result != nullptr, "Entity doesn't have this component");

					return result;
				}
			};
		}
	}
}

#endif