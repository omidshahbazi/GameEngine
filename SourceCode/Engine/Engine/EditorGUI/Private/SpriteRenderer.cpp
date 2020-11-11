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
				m_IsDirty(false)
			{
				SetMaterial(*Resources::GetInstance()->GetSpriteRendererMaterial());

				SetColor(ColorUI8::White);
				SetDrawMode(DrawModes::Simple);
				SetSprite(ResourceManager::GetInstance()->GetWhiteSprite());
			}

			void SpriteRenderer::Update(void)
			{
				if (m_IsDirty)
				{
					static Pass::ConstantHash ConstantHash_difText = Pass::GetHash("difTex");
					static Pass::ConstantHash ConstantHash_texDim = Pass::GetHash("texDim");
					static Pass::ConstantHash ConstantHash_texBorders = Pass::GetHash("texBorders");

					GetPass().SetSprite(ConstantHash_difText, m_Sprite);

					if (m_Sprite->IsNull())
						return;

					auto& dimension = (*m_Sprite)->GetDimension();
					GetPass().SetVector2(ConstantHash_texDim, Vector2F(dimension.X, dimension.Y));

					auto& borders = (*m_Sprite)->GetBorders();
					GetPass().SetVector4(ConstantHash_texBorders, Vector4F(borders.X, borders.Y, borders.Z, borders.W));

					m_IsDirty = false;
				}
			}

			void SpriteRenderer::Render(EditorRenderDeviceBase* Device, const Vector2I& Position) const
			{
				Device->DrawQuad(Position, GetRotation(), GetDimension(), GetMaterial());
			}

			void SpriteRenderer::SetColor(const ColorUI8& Value)
			{
				RendererBase::SetColor(Value);

				static Pass::ConstantHash ConstantHash_color = Pass::GetHash("color");

				GetPass().SetColor(ConstantHash_color, Value);
			}

			void SpriteRenderer::SetDrawMode(DrawModes Value)
			{
				m_DrawMode = Value;

				static Pass::ConstantHash ConstantHash_drawMode = Pass::GetHash("drawMode");

				GetPass().SetFloat32(ConstantHash_drawMode, (int32)m_DrawMode);
			}

			void SpriteRenderer::SetSprite(SpriteResource* Value)
			{
				m_Sprite = Value;

				m_IsDirty = true;
			}

			void SpriteRenderer::SetDimension(const Vector2I& Value)
			{
				RendererBase::SetDimension(Value);

				static Pass::ConstantHash ConstantHash_elemDim = Pass::GetHash("elemDim");

				GetPass().SetVector2(ConstantHash_elemDim, Vector2F(Value.X, Value.Y));
			}
		}
	}
}