// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <GameObjectSystem\Data\SceneData.h>

namespace Engine
{
	using namespace Containers;

	namespace GameObjectSystem
	{
		using namespace Data;

		namespace Private
		{
			CREATOR_DEFINITION(GameObjectSystemAllocators);

			DynamicSizeAllocator* GameObjectSystemAllocators::GameObjectSystemAllocator = nullptr;
			DynamicSizeAllocator* GameObjectSystemAllocators::SceneDataAllocator = nullptr;

			GameObjectSystemAllocators::GameObjectSystemAllocators(void)
			{
				static DynamicSizeAllocator gameObjectSystemAllocator("Game Object System Allocator", RootAllocator::GetInstance(), MegaByte * 400);
				GameObjectSystemAllocator = &gameObjectSystemAllocator;

				static DynamicSizeAllocator sceneDataAllocator("Scene Data Allocator", &gameObjectSystemAllocator, sizeof(SceneData) * MAX_SCENE_COUNT);
				SceneDataAllocator = &sceneDataAllocator;
			}
		}
	}
}