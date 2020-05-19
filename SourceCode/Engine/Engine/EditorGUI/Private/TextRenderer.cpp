﻿// Copyright 2016-2020 ?????????????. All Rights Reserved.
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

				auto drawCallback = [&](Mesh* Mesh, TextureHandle* Texture, const Matrix4F& Model)
				{
					GetPass().SetTexture(StringRenderer::FONT_TEXTURE_CONSTANT_NAME, Texture);

					Device->DrawMesh(Mesh, Model, GetMaterial());
				};

				Matrix4F modelMat(Matrix4F::Identity);
				modelMat.SetTranslate(Vector3F(Position.X, Position.Y + GetDimension().Y, 0));

				StringRenderer::Render(drawCallback, modelMat, m_Text, m_Font, m_Size, 1);
			}

			void TextRenderer::SetColor(const ColorUI8& Value)
			{
				RendererBase::SetColor(Value);

				GetPass().SetColor("color", Value);
			}
		}
	}
}