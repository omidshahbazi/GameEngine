// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GAME_OBJECT_SYSTEM_ALLOCATORS_H
#define GAME_OBJECT_SYSTEM_ALLOCATORS_H

#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace GameObjectSystem
	{
		namespace Private
		{
			class GAMEOBJECTSYSTEM_API GameObjectSystemAllocators
			{
			public:
				static const uint32 MAX_SCENE_COUNT = 10;
				static const uint32 MAX_GAME_OBJECT_COUNT = 100000;

				static DynamicSizeAllocator GameObjectSystemAllocator;
				static DynamicSizeAllocator SceneDataAllocator;
			};
		}
	}
}

#endif