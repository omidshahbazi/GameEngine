// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <FontSystem\FontManager.h>
#include <FontSystem\Font.h>
#include <FontSystem\Private\FontSystemAllocators.h>
#include <Allocators\RootAllocator.h>
#include <RenderSystem\RenderManager.h>
#include <RenderSystem\Texture.h>
#include <RenderSystem\Mesh.h>

namespace Engine
{
	using namespace Containers;
	using namespace RenderSystem;

	namespace FontSystem
	{
		using namespace Private;

		SINGLETON_DEFINITION(FontManager);

		FontManager::FontManager(void) :
			m_QuadMesh(nullptr)
		{
			FontSystemAllocators::Create();

			CreateQuadMesh();
		}

		FontManager::~FontManager(void)
		{
			RenderManager::GetInstance()->GetDevice()->DestroyMesh(m_QuadMesh);
		}

		Font* FontManager::CreateFont(const FontInfo* Info)
		{
			Font* font = FontSystemAllocators::FontSystemAllocator_Allocate<Font>();
			Construct(font);

			DeviceInterface* device = RenderManager::GetInstance()->GetDevice();

			font->m_Size = Info->Size;
			font->m_RenderType = Info->RenderType;

			Texture* texture = nullptr;
			if (font->m_RenderType == FontRenderTypes::Texture)
			{
				texture = device->CreateTexture(&Info->TextureInfo);
				texture->SetMinifyFilter(TextureMinifyFilters::Linear);
				texture->SetMagnifyFilter(TextureMagnfyFilters::Linear);
			}

			font->m_Texture = texture;

			for (const auto& glyphInfo : Info->Glyphs)
			{
				auto& ch = font->m_Characters[glyphInfo.CharCode];

				ch.m_Size = glyphInfo.Size;
				ch.m_Bearing = glyphInfo.Bearing;
				ch.m_Advance = glyphInfo.Advance;

				if (font->m_RenderType == FontRenderTypes::Mesh)
				{
					ch.m_Mesh = nullptr;

					if (glyphInfo.MeshInfo.SubMeshes.GetSize() != 0)
						ch.m_Mesh = device->CreateMesh(&glyphInfo.MeshInfo);
				}
				else if (font->m_RenderType == FontRenderTypes::Texture)
				{
					ch.SetMesh(m_QuadMesh);
					ch.SetTexture(texture);
					ch.m_Bounds = glyphInfo.Bounds;
				}
			}

			return font;
		}

		void FontManager::DestroyFont(Font* Font)
		{
			FontSystemAllocators::FontSystemAllocator_Deallocate(Font);
		}

		void FontManager::CreateQuadMesh(void)
		{
			SubMeshInfo subMeshInfo;

			subMeshInfo.Vertices.Add({ Vector3F(-0.5F, 0.5F, 0), Vector2F(0, 1) });
			subMeshInfo.Vertices.Add({ Vector3F(-0.5F, -0.5F, 0), Vector2F(0, 0) });
			subMeshInfo.Vertices.Add({ Vector3F(0.5F, 0.5F, 0), Vector2F(1, 1) });
			subMeshInfo.Vertices.Add({ Vector3F(0.5F, -0.5F, 0), Vector2F(1, 0) });
			subMeshInfo.Indices.Add(0);
			subMeshInfo.Indices.Add(1);
			subMeshInfo.Indices.Add(2);
			subMeshInfo.Indices.Add(2);
			subMeshInfo.Indices.Add(1);
			subMeshInfo.Indices.Add(3);
			subMeshInfo.Type = PolygonTypes::Triangles;
			subMeshInfo.Layout = VertexLayouts::Position | VertexLayouts::TexCoord;

			MeshInfo meshInfo;
			meshInfo.SubMeshes.Add(&subMeshInfo);

			m_QuadMesh = RenderManager::GetInstance()->GetDevice()->CreateMesh(&meshInfo);
			m_QuadMesh->SetName("FontQuadMesh");
		}
	}
}