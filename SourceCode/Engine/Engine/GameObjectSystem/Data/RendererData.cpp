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
				m_IDAllocator("ID Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(IDType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_ParentIDAllocator("Parent ID Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(IDType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_LocalMatrixAllocator("Local Matrix Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(Matrix4F) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_WorldMatrixAllocator("World Matrix Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(Matrix4F) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_ComponentMaskAllocator("Component Mask Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(ComponentMask) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT)
			{
				IDs = IDFList(&m_IDAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				ParentIDs = IDFList(&m_ParentIDAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				LocalMatrices = Matrix4FList(&m_LocalMatrixAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				WorldMatrices = Matrix4FList(&m_WorldMatrixAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				ComponentMasks = ComponentMaskList(&m_ComponentMaskAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}
		}
	}
}
