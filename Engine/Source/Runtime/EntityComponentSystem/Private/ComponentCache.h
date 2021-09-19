// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMPONENT_HASH_H
#define COMPONENT_HASH_H

#include <EntityComponentSystem\Entity.h>
#include <EntityComponentSystem\Private\Cache.h>

namespace Engine
{
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

			public:
				ComponentCache(AllocatorBase* Allocator) :
					Cache<InfoType>(Allocator, 256)
				{
				}

				template<typename... ParameterTypes>
				ComponentType* Create(const Entity& Entity, ParameterTypes&& ...Arguments)
				{
					InfoType* info = Cache<InfoType>::Allocate();

					info->BelongsTo = Entity;

					Construct(&info->Value, std::forward<ParameterTypes>(Arguments)...);

					return &info->Value;
				}
			};
		}
	}
}

#endif