// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef MAP_H
#define MAP_H

#include <map>

namespace Engine
{
	namespace Containers
	{
		template<typename K, typename V>
		using Map = std::map<K, V>;
	}
}

#endif