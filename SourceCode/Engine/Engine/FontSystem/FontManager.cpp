// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <FontSystem\FontManager.h>
#include <FontSystem\Private\FontSystemAllocators.h>
#include <FontSystem\FontLoader.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

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

		Font* FontManager::CreateFont(const FontInfo* Buffer)
		{
			Font* font = FontSystemAllocators::AllocatorReference_Allocate<Font>();
			Construct(font);


			return font;
		}

		void FontManager::DestroyFont(Font* Font)
		{
			FontSystemAllocators::AllocatorReference_Deallocate(Font);
		}
	}
} 
