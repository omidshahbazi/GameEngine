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

			auto &id = data->IDs.Allocate();
			id = data->m_LastGameObjectID;

			auto &parentID = data->ParentIDs.Allocate();
			parentID = -1;

			auto &localMat = data->LocalMatrices.Allocate();
			localMat.MakeIdentity();

			auto &worldMat = data->WorldMatrices.Allocate();
			worldMat.MakeIdentity();

			auto &comMask = data->ComponentMasks.Allocate();
			comMask = 0;

			return GameObject(m_ID, data->m_LastGameObjectID);
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

			for (uint32 i = 0; i < data->LocalMatrices.GetSize(); ++i)
				data->WorldMatrices[i] = ViewProjection * data->LocalMatrices[i];
		}

		SceneData *Scene::GetScene(void)
		{
			return SceneManager::GetInstance()->GetScene(m_ID);
		}
	}
}