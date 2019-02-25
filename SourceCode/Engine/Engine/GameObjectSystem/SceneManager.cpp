// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\SceneManager.h>
#include <GameObjectSystem\Data\SceneData.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		using namespace Private;

		SINGLETON_DEFINITION(SceneManager)

		SceneManager::SceneManager(void) :
			m_Scenes(&GameObjectSystemAllocators::SceneDataAllocator, GameObjectSystemAllocators::MAX_SCENE_COUNT),
			m_LastID(0)
		{
		}

		SceneManager::~SceneManager(void)
		{
		}

		Scene SceneManager::CreateScene(void)
		{
			++m_LastID;

			SceneData &data = m_Scenes.Allocate();

			new (&data) SceneData();

			data.ID = m_LastID;

			return Scene(data.ID);
		}

		SceneData *SceneManager::GetScene(IDType ID)
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