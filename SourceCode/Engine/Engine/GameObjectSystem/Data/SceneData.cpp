// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\SceneData.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			SceneData::SceneData(void) :
				m_LocalMatrixAllocator("Local Matrix Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(Matrix4F) * (GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT + 2)),
				m_WorldMatrixAllocator("World Matrix Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(Matrix4F) * (GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT + 2))
			{
				LocalMatrices = Matrix4FList(&m_LocalMatrixAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				WorldMatrices = Matrix4FList(&m_WorldMatrixAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}
		}
	}
}
