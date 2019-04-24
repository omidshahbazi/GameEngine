// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\TextRenderer.h>
#include <GameObjectSystem\SceneManager.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		TextRenderer::TextRenderer(IDType SceneID, IDType ID, GameObjectTypes Type) :
			Component(SceneID, ID, Type)
		{
		}

		void TextRenderer::SetFont(FontHandle * Font)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->TextRenderables.TextRenderers.SetFont(m_ID, Font);
		}

		void TextRenderer::SetText(const WString & Text)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->TextRenderables.TextRenderers.SetText(m_ID, Text);
		}

		void TextRenderer::SetMaterial(Material * Material)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->TextRenderables.TextRenderers.SetMaterial(m_ID, Material);
		}
	}
}