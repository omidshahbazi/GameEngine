// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMPONENT_TYPE_TRAITS_H
#define COMPONENT_TYPE_TRAITS_H

#include <Common\PrimitiveTypes.h>
#include <type_traits>

namespace Engine
{
	using namespace Common;

	namespace EntityComponentSystem
	{
		namespace Private
		{
			class ComponentTypeTraits
			{
			public:
				template<typename... ComponentTypes>
				static uint32 GetTypeID(void)
				{
					static uint32 id = (typeid(std::remove_const<ComponentTypes>::type).hash_code() + ...);

					return id;
				}
			};
		}
	}
}

#endif