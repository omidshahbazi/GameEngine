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

		SceneManager::SceneManager(void)
		{
		}

		Scene SceneManager::CreateScene(void)
		{
			SceneData *data = m_DataManager.CreateScene();

			return Scene(data->ID);
		}

		SceneData *SceneManager::GetScene(IDType ID)
		{
			return m_DataManager.GetScene(ID);
		}
	}
}