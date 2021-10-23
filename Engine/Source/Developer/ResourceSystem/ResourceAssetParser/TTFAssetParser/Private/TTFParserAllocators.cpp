// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <TTFAssetParser\Private\TTFParserAllocators.h>
#include <Allocators\RootAllocator.h>

namespace Engine
{
	namespace ResourceAssetParser
	{
		namespace Private
		{
			CREATOR_DEFINITION(TTFParserAllocators);

			DynamicSizeAllocator* TTFParserAllocators::MeshGeneratorAllocator = nullptr;

			TTFParserAllocators::TTFParserAllocators(void)
			{
				static DynamicSizeAllocator meshGeneratorAllocator("Mesh Generator Allocator", RootAllocator::GetInstance(), 128 * MegaByte);
				MeshGeneratorAllocator = &meshGeneratorAllocator;
			}
		}
	}
}