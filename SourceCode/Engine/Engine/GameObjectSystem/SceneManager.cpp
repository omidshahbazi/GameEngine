// Copyright 2016-2020 ?????????????. All Rights Reserved.
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
			m_ActiveScene(0)
		{
			GameObjectSystemAllocators::Create();
		}

		SceneManager::~SceneManager(void)
		{
		}

		Scene SceneManager::CreateScene(void)
		{
			IDType id = m_DataManager.Create();

			return Scene(id);
		}

		Scene SceneManager::GetActiveScene(void) const
		{
			return Scene(m_ActiveScene);
		}

		SceneData *SceneManager::GetSceneData(IDType ID)
		{
			return m_DataManager.GetScene(ID);
		}
	}
}