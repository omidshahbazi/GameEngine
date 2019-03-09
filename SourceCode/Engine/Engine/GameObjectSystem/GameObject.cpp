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
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			auto id = sceneData->Transforms.Create();
			sceneData->Transforms.SetGameObjectID(id, m_ID);
			sceneData->GameObjects.SetComponentFlagState(m_ID, GameObjectDataManager::ComponentTypes::Transform, true);
		}

		Camera GameObject::AddCamera(void)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			auto id = sceneData->Cameras.Create();

			sceneData->Cameras.SetGameObjectID(id, m_ID);

			sceneData->GameObjects.SetComponentFlagState(m_ID, GameObjectDataManager::ComponentTypes::Camera, true);

			return Camera(m_SceneID, id);
		}

		Renderer GameObject::AddRenderer(void)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			auto id = sceneData->Renderers.Create();

			sceneData->Renderers.SetGameObjectID(id, m_ID);

			sceneData->GameObjects.SetComponentFlagState(m_ID, GameObjectDataManager::ComponentTypes::Renderer, true);

			return Renderer(m_SceneID, id);
		}
	}
}