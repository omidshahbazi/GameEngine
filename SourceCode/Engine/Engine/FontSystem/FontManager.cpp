// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <FontSystem\FontManager.h>
#include <FontSystem\Private\FontSystemAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Rendering\RenderingManager.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering;

	namespace FontSystem
	{
		using namespace Private;

		SINGLETON_DEFINITION(FontManager)

			FontManager::FontManager(void)
		{
		}

		FontManager::~FontManager(void)
		{
			//TODO: deallocate all fonts;
		}

		Font* FontManager::CreateFont(const FontInfo* Info)
		{
			Font* font = FontSystemAllocators::AllocatorReference_Allocate<Font>();
			Construct(font);

			DeviceInterface* device = RenderingManager::GetInstance()->GetActiveDevice();

			font->m_RenderType = Info->RenderType;

			Texture* texture = nullptr;
			if (font->m_RenderType == Font::RenderTypes::Texture)
			{
				texture = device->CreateTexture(&Info->TextureInfo);
				texture->SetMinifyFilter(Texture::MinifyFilters::Linear);
				texture->SetMagnifyFilter(Texture::MagnfyFilters::Linear);
			}

			for each (const auto & glyphInfo in Info->Glyphs)
			{
				auto& ch = font->m_Characters[glyphInfo.CharCode];

				ch.m_Size = glyphInfo.Size;
				ch.m_Bearing = glyphInfo.Bearing;
				ch.m_Advance = glyphInfo.Advance;

				if (font->m_RenderType == Font::RenderTypes::Mesh)
				{
					ch.m_Mesh = nullptr;

					if (glyphInfo.MeshInfo.SubMeshes.GetSize() != 0)
						ch.m_Mesh = device->CreateMesh(&glyphInfo.MeshInfo, GPUBuffer::Usages::StaticDraw);
				}
				else if (font->m_RenderType == Font::RenderTypes::Texture)
				{
					ch.SetTexture(texture);
				}
			}

			return font;
		}

		void FontManager::DestroyFont(Font* Font)
		{
			FontSystemAllocators::AllocatorReference_Deallocate(Font);
		}
	}
}