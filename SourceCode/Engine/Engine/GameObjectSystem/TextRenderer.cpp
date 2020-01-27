// Copyright 2016-2020 ?????????????. All Rights Reserved.
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

		void TextRenderer::SetMaterial(Material * Material)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->TextRenderables.TextRenderers.SetMaterial(m_ID, Material);
		}

		void TextRenderer::SetText(const WString & Text)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->TextRenderables.TextRenderers.SetText(m_ID, Text);
		}

		void TextRenderer::SetRightToLeft(bool RightToLeft)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->TextRenderables.TextRenderers.SetRightToLeft(m_ID, RightToLeft);
		}

		void TextRenderer::SetSize(float32 Size)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->TextRenderables.TextRenderers.SetSize(m_ID, Size);
		}

		void TextRenderer::SetOutlineThicknes(float32 OutlineThickness)
		{
			SceneData *sceneData = SceneManager::GetInstance()->GetSceneData(m_SceneID);

			sceneData->TextRenderables.TextRenderers.SetOutlineThicknes(m_ID, OutlineThickness);
		}
	}
}