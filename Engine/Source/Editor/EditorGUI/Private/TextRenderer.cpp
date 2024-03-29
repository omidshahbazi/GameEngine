// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\TextRenderer.h>
#include <EditorGUI\Private\Resources.h>
#include <EditorGUI\EditorRenderCommandBuffer.h>
#include <FontSystem\StringRenderer.h>

namespace Engine
{
	using namespace ResourceSystem;
	using namespace RenderSystem;

	namespace EditorGUI
	{
		namespace Private
		{
			TextRenderer::TextRenderer(void) :
				m_Font(nullptr),
				m_Size(10),
				m_IsMultiLine(false),
				m_LineSpacing(0)
			{
				SetMaterial(Resources::GetInstance()->GetTextRendererMaterial());

				SetColor(ColorUI8::White);

				m_Font = Resources::GetInstance()->GetFont();
			}

			void TextRenderer::Update(void)
			{
				uint32 textLen = m_Text.GetLength();
				if (textLen > m_Materials.GetSize())
				{
					m_Materials.Clear();
					m_Materials.Extend(textLen);

					const auto& refMat = GetMaterial();

					Material* materials = m_Materials.GetData();
					for (uint32 i = 0; i < textLen; ++i)
						materials[i] = refMat;
				}
			}

			void TextRenderer::Render(EditorRenderCommandBuffer* CommandBuffer, const Vector2I& Position) const
			{
				if (m_Text == WString::Empty)
					return;

				static const ProgramConstantHash ConstantHash_FontTexture = Material::GetHash("FontTexture");
				static const ProgramConstantHash ConstantHash_data = Material::GetHash("data");

				uint32 index = 0;
				auto drawCallback = [&](const Font::Character* Character, const Matrix4F& Model)
				{
					TextRenderer* renderer = ConstCast(TextRenderer*, this);

					renderer->m_Data.FontTextureBound = Character->GetBounds();

					Material& material = renderer->m_Materials[index++];
					material.SetTexture(ConstantHash_FontTexture, Character->GetTexture());
					material.SetBuffer(ConstantHash_data, &m_Data);

					CommandBuffer->DrawMesh(Character->GetMesh(), Model, &material);
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

				index = 0;
				StringRenderer::Render(drawCallback, modelMat, m_Text, &info);
			}

			void TextRenderer::SetColor(const ColorUI8& Value)
			{
				static const ProgramConstantHash ConstantHash_data = Material::GetHash("data");

				RendererBase::SetColor(Value);

				(ColorF&)m_Data.Color << Value;
				GetMaterial().SetBuffer(ConstantHash_data, &m_Data);
			}
		}
	}
}