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
			GameObjectDataManager::GameObjectDataManager(SceneData *SceneData) :
				DataManagerBase(SceneData),
				m_ParentIDAllocator("Parent ID Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(IDFList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT)
			{
				m_ParentIDs = IDFList(&m_ParentIDAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType GameObjectDataManager::Create(void)
			{
				auto id = DataManagerBase::Create();

				auto &parentID = m_ParentIDs.Allocate();
				parentID = -1;

				return id;
			}
		}
	}
}
