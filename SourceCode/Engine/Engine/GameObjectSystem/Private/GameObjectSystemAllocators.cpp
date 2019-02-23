// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <GameObjectSystem\Scene.h>

namespace Engine
{
	using namespace Containers;

	namespace GameObjectSystem
	{
		using namespace Data;

		namespace Private
		{
			DynamicSizeAllocator GameObjectSystemAllocators::GameObjectSystemAllocator("Game Object System Allocator", RootAllocator::GetInstance(), MegaByte * 500);
			DynamicSizeAllocator GameObjectSystemAllocators::SceneDataAllocator("Scene Data Allocator", &GameObjectSystemAllocator, sizeof(SceneData) * (MAX_SCENE_COUNT + 2));
		}
	}
}