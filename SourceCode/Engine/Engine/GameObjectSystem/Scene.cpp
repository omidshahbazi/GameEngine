// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Scene.h>
#include <GameObjectSystem\GameObject.h>
#include <GameObjectSystem\SceneManager.h>
#include <GameObjectSystem\Data\SceneData.h>
#include <GameObjectSystem\GameObjectSystemCommon.h>
#include <Profiler\Profiling.h>

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

		GameObject Scene::CreateLightingGameObject(void)
		{
			SceneData *data = GetScene();

			IDType goID = data->Lightings.GameObjects.Create();
			data->Lightings.Transforms.Create();
			data->Lightings.Lights.Create();

			return GameObject(m_ID, goID, GameObjectTypes::Light);
		}

		GameObject Scene::CreateTextRenderableGameObject(void)
		{
			SceneData *data = GetScene();

			IDType goID = data->TextRenderables.GameObjects.Create();
			data->TextRenderables.Transforms.Create();
			data->TextRenderables.TextRenderers.Create();

			return GameObject(m_ID, goID, GameObjectTypes::TextRenderable);
		}

		void Scene::Update(void)
		{
			ProfileFunction();

			SceneData *data = GetScene();

			SceneData::CamerasObjects &camObj = data->Cameras;
			camObj.GameObjects.Update();
			camObj.Transforms.Update();
			camObj.Cameras.Update();

			SceneData::RenderableObjects &renObj = data->Renderables;
			renObj.GameObjects.Update();
			renObj.Transforms.Update();
			renObj.Renderers.Update();

			SceneData::LightinngObjects &lightObj = data->Lightings;
			lightObj.GameObjects.Update();
			lightObj.Transforms.Update();
			lightObj.Lights.Update();

			SceneData::TextRenderableObjects &textRenObj = data->TextRenderables;
			textRenObj.GameObjects.Update();
			textRenObj.Transforms.Update();
			textRenObj.TextRenderers.Update();
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

			SceneData::LightinngObjects &lightObj = data->Lightings;
			lightObj.GameObjects.Render();
			lightObj.Transforms.Render();
			lightObj.Lights.Render();

			SceneData::TextRenderableObjects &textRenObj = data->TextRenderables;
			textRenObj.GameObjects.Render();
			textRenObj.Transforms.Render();
			textRenObj.TextRenderers.Render();
		}

		SceneData *Scene::GetScene(void)
		{
			return SceneManager::GetInstance()->GetSceneData(m_ID);
		}
	}
}