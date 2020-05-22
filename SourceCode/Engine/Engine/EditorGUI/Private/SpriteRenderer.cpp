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
			SpriteRenderer::SpriteRenderer(void)
			{
				SetMaterial(*Resources::GetSpriteRendererMaterial());

				SetColor(ColorUI8::White);
				SetDrawMode(DrawModes::Simple);
				SetSprite(ResourceManager::GetInstance()->GetWhiteSprite().GetData());
			}

			void SpriteRenderer::Render(EditorRenderDeviceBase* Device, const Vector2I& Position) const
			{
				Device->DrawQuad(Position, GetRotation(), GetDimension(), GetMaterial());
			}

			void SpriteRenderer::SetColor(const ColorUI8& Value)
			{
				RendererBase::SetColor(Value);

				GetPass().SetColor("color", Value);
			}

			void SpriteRenderer::SetDrawMode(DrawModes Value)
			{
				m_DrawMode = Value;

				GetPass().SetFloat32("drawMode", (int32)m_DrawMode);
			}

			void SpriteRenderer::SetSprite(SpriteHandle* Value)
			{
				m_Sprite = Value;

				GetPass().SetSprite("difTex", m_Sprite);

				if (m_Sprite == nullptr || m_Sprite->IsNull())
					return;

				auto& dimension = m_Sprite->GetData()->GetDimension();
				GetPass().SetVector2("texDim", Vector2F(dimension.X, dimension.Y));

				auto& borders = m_Sprite->GetData()->GetBorders();
				GetPass().SetVector4("texBorders", Vector4F(borders.X, borders.Y, borders.Z, borders.W));
			}

			void SpriteRenderer::SetDimension(const Vector2I& Value)
			{
				RendererBase::SetDimension(Value);

				GetPass().SetVector2("elemDim", Vector2F(Value.X, Value.Y));
			}
		}
	}
}