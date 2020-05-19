// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\StringRenderer.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Material.h>

namespace Engine
{
	using namespace Rendering;

	namespace FontSystem
	{
		const String StringRenderer::FONT_TEXTURE_CONSTANT_NAME = "_FontTexture";
		Mesh* StringRenderer::m_QuadMesh = nullptr;

		void StringRenderer::Render(DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, Font* Font, float32 Size, float32 Alignment)
		{
			if (Font->GetRenderType() == Font::RenderTypes::Mesh)
				RenderMeshSting(DrawCallback, Model, Text, Font, Size, Alignment);
			else if (Font->GetRenderType() == Font::RenderTypes::Texture)
				RenderTextureString(DrawCallback, Model, Text, Font, Size, Alignment);
		}

		void StringRenderer::Render(DeviceInterface* Device, const Matrix4F& Model, const Matrix4F& Projection, const WString& Text, Font* Font, Material* Material, float32 Size, float32 Alignment)
		{
			auto drawMeshCallback = [&](Mesh* Mesh, TextureHandle* Texture, const Matrix4F& Model)
			{
				Matrix4F mvp = Projection * Model;

				if (Font->GetRenderType() == Font::RenderTypes::Texture)
					Material->GetPasses()[0].SetTexture(FONT_TEXTURE_CONSTANT_NAME, Texture);

				Device->DrawMesh(Mesh, Model, Matrix4F::Identity, Projection, mvp, Material);
			};

			Render(drawMeshCallback, Model, Text, Font, Size, Alignment);
		}

		void StringRenderer::RenderMeshSting(StringRenderer::DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, Font* Font, float32 Size, float32 Alignment)
		{
			// Glyphs are really large
			// TODO: check out size in loading time after moving to ResourceFactory
			Size /= 40;

			Matrix4F charMat(Matrix4F::Identity);
			charMat.SetScale({ Size, Size, 1 });

			float32 maxYAdvance = 0.0F;
			float32 sumYAdvance = 0.0F;
			float32 sumXAdvance = 0.0F;
			for (uint32 i = 0; i < Text.GetLength(); ++i)
			{
				char16 charCode = Text[i];

				if (charCode == '\n' || charCode == '\r')
				{
					sumYAdvance -= maxYAdvance;
					sumXAdvance = 0;
					maxYAdvance = 0;

					continue;
				}

				Font::Character* ch = Font->GetCharacter(Text[i]);

				if (ch == nullptr)
				{
					ch = Font->GetCharacter('?');

					if (ch == nullptr)
						continue;
				}

				Vector2F bearing = ch->GetBearing() * Size * Alignment;
				Vector2F advance = ch->GetAdvance() * Size * Alignment;

				auto mesh = ch->GetMesh();
				if (mesh != nullptr)
				{
					Matrix4F charMat(Matrix4F::Identity);
					charMat.SetTranslate({ sumXAdvance + bearing.X, sumYAdvance + bearing.Y, 0 });
					charMat.SetScale({ Size, -Size, 1 });
					charMat = Model * charMat;

					DrawCallback(ch->GetMesh(), nullptr, charMat);
				}

				sumXAdvance += advance.X;

				if (maxYAdvance < ch->GetSize().Y)
					maxYAdvance = ch->GetSize().Y;
			}
		}

		void StringRenderer::RenderTextureString(StringRenderer::DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, Font* Font, float32 Size, float32 Alignment)
		{
			CreateMeshIfIsNull();

			Matrix4F charMat(Matrix4F::Identity);
			charMat.SetScale({ Size, Size, 1 });

			float32 maxYAdvance = 0.0F;
			float32 sumYAdvance = 0.0F;
			float32 sumXAdvance = 0.0F;
			for (uint32 i = 0; i < Text.GetLength(); ++i)
			{
				char16 charCode = Text[i];

				if (charCode == '\n' || charCode == '\r')
				{
					sumYAdvance -= maxYAdvance;
					sumXAdvance = 0;
					maxYAdvance = 0;

					continue;
				}

				Font::Character* ch = Font->GetCharacter(Text[i]);

				if (ch == nullptr)
				{
					ch = Font->GetCharacter('?');

					if (ch == nullptr)
						continue;
				}

				//Vector2F bearing = ch->GetBearing() * Size * Alignment;
				//Vector2F advance = ch->GetAdvance() * Size * Alignment;

				Vector2F bearing = Vector2F::Zero;

				if (ch->GetTexture() != nullptr)
				{
					Matrix4F charMat(Matrix4F::Identity);
					charMat.SetTranslate({ sumXAdvance + bearing.X, sumYAdvance + bearing.Y, 0 });
					charMat.SetScale({ Size, Size, 1 });
					charMat = Model * charMat;

					DrawCallback(m_QuadMesh, ch->GetTexture(), charMat);
				}

				//sumXAdvance += advance.X;

				if (maxYAdvance < ch->GetSize().Y)
					maxYAdvance = ch->GetSize().Y;
			}
		}

		void StringRenderer::CreateMeshIfIsNull(void)
		{
			if (m_QuadMesh != nullptr)
				return;

			SubMeshInfo subMeshInfo;
			subMeshInfo.Vertices.Add(Vertex(Vector3F(-0.5F, -0.5F, 0), Vector2F(0, 0)));
			subMeshInfo.Vertices.Add(Vertex(Vector3F(-0.5F, 0.5F, 0), Vector2F(0, 1)));
			subMeshInfo.Vertices.Add(Vertex(Vector3F(0.5F, 0.5F, 0), Vector2F(1, 1)));
			subMeshInfo.Vertices.Add(Vertex(Vector3F(0.5F, -0.5F, 0), Vector2F(1, 0)));
			subMeshInfo.Indices.Add(0);
			subMeshInfo.Indices.Add(1);
			subMeshInfo.Indices.Add(2);
			subMeshInfo.Indices.Add(1);
			subMeshInfo.Indices.Add(3);
			subMeshInfo.Indices.Add(0);
			subMeshInfo.Layout = SubMesh::VertexLayouts::Position | SubMesh::VertexLayouts::UV;

			MeshInfo meshInfo;
			meshInfo.SubMeshes.Add(&subMeshInfo);

			m_QuadMesh = RenderingManager::GetInstance()->GetActiveDevice()->CreateMesh(&meshInfo, GPUBuffer::Usages::StaticDraw);
		}
	}
}