// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Renderer.h>
#include <GameObjectSystem\SceneManager.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		Renderer::Renderer(IDType SceneID, IDType ID, GameObjectTypes Type) :
			Component(SceneID, ID, Type)
		{
		}

		void Renderer::SetMesh(MeshHandle * Mesh)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->Renderables.Renderers.SetMesh(m_ID, Mesh);
		}

		void Renderer::SetMaterial(Material * Material)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->Renderables.Renderers.SetMaterial(m_ID, Material);
		}
	}
}