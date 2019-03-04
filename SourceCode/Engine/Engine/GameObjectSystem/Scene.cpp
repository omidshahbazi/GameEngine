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

			IDType goID = data->GameObjects.CreateGameObject();

			return GameObject(m_ID, goID);
		}

		void Scene::Update(void)
		{
			static Matrix4F mat;
			mat.MakeIdentity();

			SceneData *data = GetScene();

			data->GameObjects.UpdateWorldMatrices(mat);
		}

		void Scene::Render(void)
		{
			SceneData *data = GetScene();

			data->Renderers.Render();
		}

		SceneData *Scene::GetScene(void)
		{
			return SceneManager::GetInstance()->GetScene(m_ID);
		}
	}
}