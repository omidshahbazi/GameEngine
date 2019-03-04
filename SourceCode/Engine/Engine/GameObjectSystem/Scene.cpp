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
			Matrix4F mat;
			mat.MakeIdentity();

			UpdateWorldMatrices(mat);
		}

		void Scene::UpdateWorldMatrices(const Matrix4F &ViewProjection)
		{
			SceneData *data = GetScene();

			data->GameObjects.UpdateWorldMatrices(ViewProjection);
		}

		SceneData *Scene::GetScene(void)
		{
			return SceneManager::GetInstance()->GetScene(m_ID);
		}
	}
}