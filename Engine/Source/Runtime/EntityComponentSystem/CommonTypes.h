// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace EntityComponentSystem
	{
		template<typename... ComponentTypes>
		struct ComponentTypeList
		{
		public:
			using Types = ComponentTypeList;
			static constexpr auto Size = sizeof...(ComponentTypes);
		};

		template<typename... ComponentTypes>
		struct ExcludeComponentTypeList : ComponentTypeList<ComponentTypes...>
		{
		};

		template<typename... ComponentTypes>
		inline constexpr ExcludeComponentTypeList<ComponentTypes...> Exclude{};
	}
}

#endif