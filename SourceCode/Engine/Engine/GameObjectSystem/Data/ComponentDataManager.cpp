// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\ComponentDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>

namespace Engine
{
	using namespace Rendering;
	using namespace MemoryManagement::Allocator;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			ComponentDataManager::ComponentDataManager(SceneData *SceneData) :
				DataManagerBase(SceneData),
				m_GameObjectIDsAllocator("Renderer GameObject IDs Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(IDFList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT)
			{
				m_GameObjectIDs = IDFList(&m_GameObjectIDsAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType ComponentDataManager::Create(void)
			{
				auto id = DataManagerBase::Create();

				m_GameObjectIDs.Allocate();

				return id;
			}

			void ComponentDataManager::SetGameObjectID(IDType ID, IDType GameObjectID)
			{
				int32 index = GetIndex(ID);

				m_GameObjectIDs[index] = GameObjectID;
			}

			int32 ComponentDataManager::GetIndexByGameObjectID(IDType GameObjectID) const
			{
				for (uint32 i = 0; i < m_GameObjectIDs.GetSize(); ++i)
					if (m_GameObjectIDs[i] == GameObjectID)
						return i;

				return -1;
			}
		}
	}
}