// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\GameObject.h>
#include <GameObjectSystem\SceneManager.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		GameObject::GameObject(IDType SceneID, IDType GameObjectID) :
			m_SceneID(SceneID),
			m_ID(GameObjectID)
		{
		}

		Camera GameObject::GetCamera(void)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			int32 index = sceneData->Cameras.GameObjects.GetIndex(m_ID);

			return Camera(m_SceneID, sceneData->Cameras.Cameras.m_IDs[index]);
		}

		Renderer GameObject::GetRenderer(void)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			int32 index = sceneData->Renderables.GameObjects.GetIndex(m_ID);

			return Renderer(m_SceneID, sceneData->Renderables.Renderers.m_IDs[index]);
		}
	}
}