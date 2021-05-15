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


			//struct Data
			//{
			//public:
			//	float32 DrawMode;
			//	Vector2F ElementDimension;

			//	float32 pad1[1];

			//	Vector2F TextureDimension;

			//	float32 pad2[2];

			//	Vector4F TextureBorders;
			//	ColorF Color;
			//};

			//struct Data
			//{
			//public:
			//	float32 DrawMode;

			//	float32 ElementDimensionX;
			//	float32 ElementDimensionY;

			//	float32 pad1[1];

			//	float32 TextureDimensionX;
			//	float32 TextureDimensionY;

			//	float32 pad2[2];

			//	float32 TextureBordersX;
			//	float32 TextureBordersY;
			//	float32 TextureBordersZ;
			//	float32 TextureBordersW;

			//	float32 ColorR;
			//	float32 ColorG;
			//	float32 ColorB;
			//	float32 ColorA;
			//};

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

				//if (m_IsDirty)
				//{
				//	static const Pass::ConstantHash ConstantHash_DiffuseTexture = Pass::GetHash("DiffuseTexture");
				//	static const Pass::ConstantHash ConstantHash_data = Pass::GetHash("data");

				//	GetPass().SetSprite(ConstantHash_DiffuseTexture, m_Sprite);

				//	if (m_Sprite->IsNull())
				//		return;

				//	auto& dimension = (*m_Sprite)->GetDimension();
				//	auto& borders = (*m_Sprite)->GetBorders();

				//	auto constantBuffer = GetPass().GetConstantBuffer(ConstantHash_data);
				//	constantBuffer->Lock(GPUBuffer::Access::WriteOnly);
				//	Data* data = constantBuffer->Get<Data>();
				//	data->TextureDimensionX = dimension.X;
				//	data->TextureDimensionY = dimension.Y;

				//	data->TextureBordersX = borders.X;
				//	data->TextureBordersY = borders.Y;
				//	data->TextureBordersZ = borders.Z;
				//	data->TextureBordersW = borders.W;

				//	constantBuffer->Unlock();

				//	m_IsDirty = false;
				//}
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
				(ColorF&)data->Color << Value;
				constantBuffer->Unlock();

				//static const Pass::ConstantHash ConstantHash_data = Pass::GetHash("data");
				//auto constantBuffer = GetPass().GetConstantBuffer(ConstantHash_data);
				//constantBuffer->Lock(GPUBuffer::Access::WriteOnly);
				//Data* data = constantBuffer->Get<Data>();
				//data->ColorR = Value.R;
				//data->ColorG = Value.G;
				//data->ColorB = Value.B;
				//data->ColorA = Value.A;
				//constantBuffer->Unlock();
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

				//static const Pass::ConstantHash ConstantHash_data = Pass::GetHash("data");
				//auto constantBuffer = GetPass().GetConstantBuffer(ConstantHash_data);
				//constantBuffer->Lock(GPUBuffer::Access::WriteOnly);
				//Data* data = constantBuffer->Get<Data>();
				//data->ElementDimensionX = Value.X;
				//data->ElementDimensionY = Value.Y;
				//constantBuffer->Unlock();
			}
		}
	}
}