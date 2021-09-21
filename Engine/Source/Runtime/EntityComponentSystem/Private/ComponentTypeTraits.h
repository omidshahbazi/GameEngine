// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMPONENT_TYPE_TRAITS_H
#define COMPONENT_TYPE_TRAITS_H

#include <Common\PrimitiveTypes.h>

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
				template<typename... TailComponentTypes>
				static uint32 GetTypeID(void)
				{
					static uint32 id = (typeid(TailComponentTypes).hash_code() + ...);

					return id;
				}
			};
		}
	}
}

#endif