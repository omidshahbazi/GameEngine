// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\GameObjectDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>

namespace Engine
{
	using namespace Allocators;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			GameObjectDataManager::GameObjectDataManager(SceneData* SceneData) :
				DataManagerBase(SceneData),
				m_ParentIDAllocator("Parent ID Allocator", GameObjectSystemAllocators::GameObjectSystemAllocator, 128 * MegaByte)
			{
				m_ParentIDs = IDFList(&m_ParentIDAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType GameObjectDataManager::Create(void)
			{
				auto id = DataManagerBase::Create();

				auto& parentID = m_ParentIDs.Allocate();
				parentID = -1;

				return id;
			}
		}
	}
}
