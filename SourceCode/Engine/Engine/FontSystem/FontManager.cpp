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

			for each (const auto & glyphInfo in Info->Glyphs)
			{
				auto& ch = font->m_Characters[glyphInfo.CharCode];

				if (ch.GetMeshInfo() != nullptr && ch.GetMeshInfo()->SubMeshes.GetSize() != 0 && ch.GetMeshInfo()->SubMeshes[0]->Vertices.GetSize() != 0)
				{
					Mesh* mesh = device->CreateMesh(ch.GetMeshInfo(), GPUBuffer::Usages::StaticDraw);
					ch.SetMesh(mesh);
				}

				m_ReadyCharacter.Add(CharCode, &ch);
			}

			return font;
		}

		void FontManager::DestroyFont(Font* Font)
		{
			FontSystemAllocators::AllocatorReference_Deallocate(Font);
		}
	}
}
