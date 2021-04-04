// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\TextRenderer.h>
#include <EditorGUI\Private\Resources.h>
#include <EditorGUI\EditorRenderDeviceBase.h>
#include <FontSystem\StringRenderer.h>
#include <Rendering\GPUAlignedType.h>

namespace Engine
{
	using namespace ResourceSystem;
	using namespace Rendering;

	namespace EditorGUI
	{
		namespace Private
		{
			struct Data
			{
			public:
				GPUAlignedVector4F FontTextureBound;
				GPUAlignedColorF Color;
			};

			TextRenderer::TextRenderer(void) :
				m_Font(nullptr),
				m_Size(10),
				m_IsMultiLine(false),
				m_LineSpacing(0)
			{
				SetMaterial(*Resources::GetInstance()->GetTextRendererMaterial());

				SetColor(ColorUI8::White);

				m_Font = Resources::GetInstance()->GetFont();
			}

			void TextRenderer::Render(EditorRenderDeviceBase* Device, const Vector2I& Position) const
			{
				if (m_Text.GetLength() == 0)
					return;

				static const Pass::ConstantHash ConstantHash_FontTexture = Pass::GetHash("FontTexture");
				static const Pass::ConstantHash ConstantHash_data = Pass::GetHash("data");

				auto constantBuffer = GetPass().GetConstantBuffer(ConstantHash_data);
				Data* data = nullptr;

				auto drawCallback = [&](const Font::Character* Character, const Matrix4F& Model)
				{
					GetPass().SetTexture(ConstantHash_FontTexture, Character->GetTexture());

					data->FontTextureBound = Character->GetBounds();

					Device->DrawMesh(Character->GetMesh(), Model, GetMaterial());
				};

				static StringRenderer::Info info;
				info.Font = m_Font->GetPointer();
				info.Size = m_Size;
				info.Alignment = 1;
				info.MultiLine = m_IsMultiLine;
				info.LineSpacing = m_LineSpacing;
				info.CharacterSpacing = 0;

				Vector2F size;
				StringRenderer::Measure(m_Text, &info, size);

				Matrix4F modelMat(Matrix4F::Identity);
				modelMat.SetTranslate(Vector3F(Position.X, Position.Y + m_Size, 0));

				constantBuffer->Lock(GPUBuffer::Access::WriteOnly);
				data = constantBuffer->Get<Data>();
				StringRenderer::Render(drawCallback, modelMat, m_Text, &info);
				constantBuffer->Unlock();
			}

			void TextRenderer::SetColor(const ColorUI8& Value)
			{
				RendererBase::SetColor(Value);

				static const Pass::ConstantHash ConstantHash_data = Pass::GetHash("data");
				auto constantBuffer = GetPass().GetConstantBuffer(ConstantHash_data);
				constantBuffer->Lock(GPUBuffer::Access::WriteOnly);
				Data* data = constantBuffer->Get<Data>();
				(ColorF32&)data->Color << Value;
				constantBuffer->Unlock();
			}
		}
	}
}