// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\TextRenderer.h>
#include <EditorGUI\Private\Resources.h>
#include <EditorGUI\EditorRenderDeviceBase.h>
#include <FontSystem\StringRenderer.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		namespace Private
		{
			TextRenderer::TextRenderer(void) :
				m_Font(nullptr),
				m_Size(1)
			{
				SetMaterial(*Resources::GetTextRendererMaterial());

				SetColor(ColorUI8::White);

				m_Font = Resources::GetFont();
			}

			void TextRenderer::Render(EditorRenderDeviceBase* Device, const Vector2I& Position) const
			{
				if (m_Font == nullptr)
					return;

				auto drawCallback = [&](const Font::Character* Character, const Matrix4F& Model)
				{
					GetPass().SetTexture(StringRenderer::FONT_TEXTURE_CONSTANT_NAME, Character->GetTexture());
					GetPass().SetVector4(StringRenderer::FONT_TEXTURE_UV_CONSTANT_NAME, Character->GetBounds());

					Device->DrawMesh(Character->GetMesh(), Model, GetMaterial());
				};

				Matrix4F modelMat(Matrix4F::Identity);
				modelMat.SetTranslate(Vector3F(Position.X, Position.Y, 0));

				static StringRenderer::Info info;
				info.Font = m_Font;
				info.Size = m_Size;
				info.Alignment = 1;
				info.LineSpacing = m_LineSpacing;

				StringRenderer::Render(drawCallback, modelMat, m_Text, &info);
			}

			void TextRenderer::SetColor(const ColorUI8& Value)
			{
				RendererBase::SetColor(Value);

				GetPass().SetColor("color", Value);
			}
		}
	}
}