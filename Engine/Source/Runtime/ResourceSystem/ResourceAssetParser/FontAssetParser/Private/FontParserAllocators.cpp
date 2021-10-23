// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <FontAssetParser\Private\FontParserAllocators.h>
#include <Allocators\RootAllocator.h>

namespace Engine
{
	namespace ResourceAssetParser
	{
		namespace Private
		{
			CREATOR_DEFINITION(FontParserAllocators);

			DynamicSizeAllocator* FontParserAllocators::MeshGeneratorAllocator = nullptr;

			FontParserAllocators::FontParserAllocators(void)
			{
				static DynamicSizeAllocator meshGeneratorAllocator("Mesh Generator Allocator", RootAllocator::GetInstance(), 128 * MegaByte);
				MeshGeneratorAllocator = &meshGeneratorAllocator;
			}
		}
	}
}