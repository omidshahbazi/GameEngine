// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Scene.h>
#include <GameObjectSystem\GameObject.h>
#include <GameObjectSystem\SceneManager.h>
#include <GameObjectSystem\Data\SceneData.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		Scene::Scene(IDType ID) :
			m_ID(ID)
		{
		}

		GameObject Scene::CreateGameObject(void)
		{
			SceneData *data = GetScene();

			++data->m_LastGameObjectID;

			data->LocalMatrices.Allocate();

			return GameObject(m_ID, data->m_LastGameObjectID);
		}

		SceneData *Scene::GetScene(void)
		{
			return SceneManager::GetInstance()->GetScene(m_ID);
		}
	}
}