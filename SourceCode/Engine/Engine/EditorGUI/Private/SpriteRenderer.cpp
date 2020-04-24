// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\SpriteRenderer.h>
#include <EditorGUI\Private\Resources.h>
#include <EditorGUI\EditorRenderDeviceBase.h>
#include <ResourceSystem\ResourceManager.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		namespace Private
		{
			SpriteRenderer::SpriteRenderer(void) :
				m_Pass(nullptr)
			{
				m_Material = *Resources::GetSpriteRendererMaterial();
				m_Pass = &m_Material.GetPasses()[0];

				SetColor(ColorUI8::White);
				SetDrawMode(DrawModes::Simple);
				SetSprite(ResourceManager::GetInstance()->GetWhiteSprite().GetData());
			}

			void SpriteRenderer::Render(EditorRenderDeviceBase* Device) const
			{
				Device->DrawMesh(Vector3F::Zero, Vector3F::Zero, m_ElementDimension3F, &m_Material);
			}

			void SpriteRenderer::SetColor(const ColorUI8& Value)
			{
				m_Color = Value;

				m_Pass->SetColor("color", m_Color);
			}

			void SpriteRenderer::SetDrawMode(DrawModes Value)
			{
				m_DrawMode = Value;

				m_Pass->SetFloat32("drawMode", (int32)m_DrawMode);
			}

			void SpriteRenderer::SetSprite(SpriteHandle* Value)
			{
				m_Sprite = Value;

				m_Pass->SetSprite("difTex", m_Sprite);

				if (m_Sprite == nullptr)
					return;

				auto& dimension = m_Sprite->GetData()->GetDimension();
				m_Pass->SetVector2("texDim", Vector2F(dimension.X, dimension.Y));

				auto& borders = m_Sprite->GetData()->GetBorders();
				m_Pass->SetVector4("texBorders", Vector4F(borders.X, borders.Y, borders.Z, borders.W));
			}

			void SpriteRenderer::SetElementDimension(const Vector2I& Value)
			{
				m_ElementDimension2I = Value;
				m_ElementDimension3F = Vector3F(Value.X, Value.Y, 0);

				m_Pass->SetVector2("elemDim", Vector2F(m_ElementDimension3F.X, m_ElementDimension3F.Y));
			}
		}
	}
}