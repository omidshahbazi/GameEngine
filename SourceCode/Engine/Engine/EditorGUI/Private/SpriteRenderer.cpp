// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\SpriteRenderer.h>
#include <EditorGUI\Private\Resources.h>
#include <EditorGUI\EditorRenderDeviceBase.h>
#include <ResourceSystem\ResourceManager.h>
#include <Rendering\GPUAlignedType.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		namespace Private
		{
			struct Data
			{
			public:
				GPUAlignedFloat32 DrawMode;
				GPUAlignedVector2F ElementDimension;
				GPUAlignedVector2F TextureDimension;
				GPUAlignedVector4F TextureBorders;
				GPUAlignedColorF Color;
			};

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
					static const Pass::ConstantHash ConstantHash_DiffuseTexture = Pass::GetHash("DiffuseTexture");
					static const Pass::ConstantHash ConstantHash_data = Pass::GetHash("data");

					GetPass().SetSprite(ConstantHash_DiffuseTexture, m_Sprite);

					if (m_Sprite->IsNull())
						return;

					auto& dimension = (*m_Sprite)->GetDimension();
					auto& borders = (*m_Sprite)->GetBorders();

					auto constantBuffer = GetPass().GetConstantBuffer(ConstantHash_data);
					constantBuffer->Lock(GPUBuffer::Access::WriteOnly);
					Data* data = constantBuffer->Get<Data>();
					data->TextureDimension = Vector2F(dimension.X, dimension.Y);
					data->TextureBorders = Vector4F(borders.X, borders.Y, borders.Z, borders.W);
					constantBuffer->Unlock();

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

				static const Pass::ConstantHash ConstantHash_data = Pass::GetHash("data");
				auto constantBuffer = GetPass().GetConstantBuffer(ConstantHash_data);
				constantBuffer->Lock(GPUBuffer::Access::WriteOnly);
				Data* data = constantBuffer->Get<Data>();
				(ColorF32&)data->Color << Value;
				constantBuffer->Unlock();
			}

			void SpriteRenderer::SetDrawMode(DrawModes Value)
			{
				m_DrawMode = Value;

				static const Pass::ConstantHash ConstantHash_data = Pass::GetHash("data");
				auto constantBuffer = GetPass().GetConstantBuffer(ConstantHash_data);
				constantBuffer->Lock(GPUBuffer::Access::WriteOnly);
				Data* data = constantBuffer->Get<Data>();
				data->DrawMode = (int32)m_DrawMode;
				constantBuffer->Unlock();
			}

			void SpriteRenderer::SetSprite(SpriteResource* Value)
			{
				m_Sprite = Value;

				m_IsDirty = true;
			}

			void SpriteRenderer::SetDimension(const Vector2I& Value)
			{
				RendererBase::SetDimension(Value);

				static const Pass::ConstantHash ConstantHash_data = Pass::GetHash("data");
				auto constantBuffer = GetPass().GetConstantBuffer(ConstantHash_data);
				constantBuffer->Lock(GPUBuffer::Access::WriteOnly);
				Data* data = constantBuffer->Get<Data>();
				data->ElementDimension = Vector2F(Value.X, Value.Y);
				constantBuffer->Unlock();
			}
		}
	}
}