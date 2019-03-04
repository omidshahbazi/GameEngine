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

		Renderer GameObject::AddRenderer(void)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			return Renderer(m_SceneID, sceneData->Renderers.Create());
		}
	}
}