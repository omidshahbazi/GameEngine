// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>
#include <Allocators\RootAllocator.h>
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
				static DynamicSizeAllocator gameObjectSystemAllocator("Game Object System Allocator", RootAllocator::GetInstance());
				GameObjectSystemAllocator = &gameObjectSystemAllocator;

				static DynamicSizeAllocator sceneDataAllocator("Scene Data Allocator", &gameObjectSystemAllocator);
				SceneDataAllocator = &sceneDataAllocator;
			}
		}
	}
}