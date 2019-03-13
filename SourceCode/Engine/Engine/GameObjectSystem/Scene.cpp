// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Scene.h>
#include <GameObjectSystem\GameObject.h>
#include <GameObjectSystem\SceneManager.h>
#include <GameObjectSystem\Data\SceneData.h>
#include <GameObjectSystem\GameObjectSystemCommon.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		Scene::Scene(IDType ID) :
			m_ID(ID)
		{
		}

		GameObject Scene::CreateCameraGameObject(void)
		{
			SceneData *data = GetScene();

			IDType goID = data->Cameras.GameObjects.Create();
			data->Cameras.Transforms.Create();
			data->Cameras.Cameras.Create();

			return GameObject(m_ID, goID, GameObjectTypes::Camera);
		}

		GameObject Scene::CreateRenderableGameObject(void)
		{
			SceneData *data = GetScene();

			IDType goID = data->Renderables.GameObjects.Create();
			data->Renderables.Transforms.Create();
			data->Renderables.Renderers.Create();

			return GameObject(m_ID, goID, GameObjectTypes::Renderable);
		}

		void Scene::Update(void)
		{
			SceneData *data = GetScene();

			SceneData::CamerasObjects &camObj = data->Cameras;

			camObj.GameObjects.Update();
			camObj.Transforms.Update();
			camObj.Cameras.Update();

			SceneData::RenderableObjects &renObj = data->Renderables;

			renObj.GameObjects.Update();
			renObj.Transforms.Update();
			renObj.Renderers.Update();
		}

		void Scene::Render(void)
		{
			SceneData *data = GetScene();

			SceneData::CamerasObjects &camObj = data->Cameras;

			camObj.GameObjects.Render();
			camObj.Transforms.Render();
			camObj.Cameras.Render();

			SceneData::RenderableObjects &renObj = data->Renderables;

			renObj.GameObjects.Render();
			renObj.Transforms.Render();
			renObj.Renderers.Render();
		}

		SceneData *Scene::GetScene(void)
		{
			return SceneManager::GetInstance()->GetSceneData(m_ID);
		}
	}
}