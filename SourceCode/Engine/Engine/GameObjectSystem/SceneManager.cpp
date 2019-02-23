// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\SceneManager.h>
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

			uint32 index = m_Scenes.Extend(1);

			SceneData &data = m_Scenes[index];

			new (&data) SceneData();

			data.ID = m_LastID;

			return Scene(data.ID);
		}
	}
}