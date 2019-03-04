// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\GameObjectDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			GameObjectDataManager::GameObjectDataManager(void) :
				m_IDAllocator("ID Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(IDFList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_ParentIDAllocator("Parent ID Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(IDFList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_LocalMatrixAllocator("Local Matrix Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(Matrix4FList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_WorldMatrixAllocator("World Matrix Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(Matrix4FList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_ComponentMaskAllocator("Component Mask Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(ComponentMaskList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT)
			{
				m_IDs = IDFList(&m_IDAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_ParentIDs = IDFList(&m_ParentIDAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_LocalMatrices = Matrix4FList(&m_LocalMatrixAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_WorldMatrices = Matrix4FList(&m_WorldMatrixAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_ComponentMasks = ComponentMaskList(&m_ComponentMaskAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType GameObjectDataManager::CreateGameObject(void)
			{
				++m_LastID;

				auto &id = m_IDs.Allocate();
				id = m_LastID;

				auto &parentID = m_ParentIDs.Allocate();
				parentID = -1;

				auto &localMat = m_LocalMatrices.Allocate();
				localMat.MakeIdentity();

				auto &worldMat = m_WorldMatrices.Allocate();
				worldMat.MakeIdentity();

				auto &comMask = m_ComponentMasks.Allocate();
				comMask = 0;

				return m_LastID;
			}

			void GameObjectDataManager::UpdateWorldMatrices(const Matrix4F &ViewProjection)
			{
				for (uint32 i = 0; i < m_LocalMatrices.GetSize(); ++i)
					m_WorldMatrices[i] = ViewProjection * m_LocalMatrices[i];
			}
		}
	}
}
