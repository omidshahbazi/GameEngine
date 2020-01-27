// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\SceneDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			SceneDataManager::SceneDataManager(void) :
				m_Scenes(&GameObjectSystemAllocators::SceneDataAllocator, GameObjectSystemAllocators::MAX_SCENE_COUNT),
				m_LastID(0)
			{
			}

			IDType SceneDataManager::Create(void)
			{
				++m_LastID;

				SceneData &data = m_Scenes.Allocate();

				new (&data) SceneData(m_LastID);

				return m_LastID;
			}

			SceneData *SceneDataManager::GetScene(IDType ID)
			{
				for (uint32 i = 0; i < m_Scenes.GetSize(); ++i)
				{
					auto &data = m_Scenes[i];

					if (data.ID == ID)
						return &data;
				}

				return nullptr;
			}
		}
	}
}
