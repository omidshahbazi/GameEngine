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

			IDType goID = data->GameObjects.Create();

			return GameObject(m_ID, goID);
		}

		void Scene::Update(void)
		{
			SceneData *data = GetScene();

			data->GameObjects.Update();
			data->Transforms.Update();
			data->Cameras.Update();
			data->Renderers.Update();
		}

		void Scene::Render(void)
		{
			SceneData *data = GetScene();

			data->GameObjects.Render();
			data->Renderers.Render();
		}

		SceneData *Scene::GetScene(void)
		{
			return SceneManager::GetInstance()->GetSceneData(m_ID);
		}
	}
}