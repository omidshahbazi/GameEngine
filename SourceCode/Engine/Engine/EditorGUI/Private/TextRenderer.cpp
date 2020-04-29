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
				m_Pass(nullptr),
				m_Font(nullptr)
			{
				m_Material = *Resources::GetTextRendererMaterial();
				m_Pass = &m_Material.GetPasses()[0];

				SetColor(ColorUI8::White);

				m_Font = Resources::GetFont();
			}

			void TextRenderer::Render(EditorRenderDeviceBase* Device, const Vector2I& Position) const
			{
				if (m_Font == nullptr)
					return;

				WString m_Text = L"Omid";

				auto drawCallback = [&](Mesh* Mesh, const Matrix4F& Model)
				{
					Device->DrawMesh(Mesh, Model, &m_Material);
				};

				StringRenderer::Render(drawCallback, Matrix4F::Identity, m_Text, m_Font, 1, 1);
			}

			void TextRenderer::SetColor(const ColorUI8& Value)
			{
				m_Color = Value;

				m_Pass->SetColor("color", m_Color);
			}
		}
	}
}