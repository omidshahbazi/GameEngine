// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\SpriteRenderer.h>
#include <EditorGUI\Private\Resources.h>
#include <EditorGUI\EditorRenderCommandBuffer.h>
#include <ResourceManagement\ResourceManager.h>

namespace Engine
{
	using namespace ResourceManagement;

	namespace EditorGUI
	{
		namespace Private
		{
			SpriteRenderer::SpriteRenderer(void) :
				m_IsDirty(false)
			{
				SetMaterial(Resources::GetInstance()->GetSpriteRendererMaterial());

				SetColor(ColorUI8::White);
				SetDrawMode(DrawModes::Simple);
				SetSprite(ResourceManager::GetInstance()->GetWhiteSprite());
			}

			void SpriteRenderer::Update(void)
			{
				if (m_IsDirty)
				{
					static const Pass::ConstantHash ConstantHash_DiffuseTexture = Pass::GetHash("DiffuseTexture");
					static const Pass::ConstantHash ConstantHash_data = Pass::GetHash("data");

					GetMaterial().SetSprite(ConstantHash_DiffuseTexture, m_Sprite);

					if (m_Sprite == nullptr || m_Sprite->IsNull())
						return;

					auto& dimension = (*m_Sprite)->GetDimension();
					m_Data.TextureDimension = Vector2F(dimension.X, dimension.Y);

					auto& borders = (*m_Sprite)->GetBorders();
					m_Data.TextureBorders = Vector4F(borders.X, borders.Y, borders.Z, borders.W);

					GetMaterial().SetBuffer(ConstantHash_data, &m_Data);

					m_IsDirty = false;
				}
			}

			void SpriteRenderer::Render(EditorRenderCommandBuffer* CommandBuffer, const Vector2I& Position) const
			{
				CommandBuffer->DrawQuad(Position, GetRotation(), GetDimension(), &GetMaterial());
			}

			void SpriteRenderer::SetColor(const ColorUI8& Value)
			{
				static const Pass::ConstantHash ConstantHash_data = Pass::GetHash("data");

				RendererBase::SetColor(Value);

				(ColorF&)m_Data.Color << Value;

				GetMaterial().SetBuffer(ConstantHash_data, &m_Data);
			}

			void SpriteRenderer::SetDrawMode(DrawModes Value)
			{
				static const Pass::ConstantHash ConstantHash_data = Pass::GetHash("data");

				m_DrawMode = Value;

				m_Data.DrawMode = (int32)m_DrawMode;

				GetMaterial().SetBuffer(ConstantHash_data, &m_Data);
			}

			void SpriteRenderer::SetSprite(SpriteResource* Value)
			{
				m_Sprite = Value;

				m_IsDirty = true;
			}

			void SpriteRenderer::SetDimension(const Vector2I& Value)
			{
				static const Pass::ConstantHash ConstantHash_data = Pass::GetHash("data");

				RendererBase::SetDimension(Value);

				m_Data.ElementDimension = Vector2F(Value.X, Value.Y);

				GetMaterial().SetBuffer(ConstantHash_data, &m_Data);
			}
		}
	}
}