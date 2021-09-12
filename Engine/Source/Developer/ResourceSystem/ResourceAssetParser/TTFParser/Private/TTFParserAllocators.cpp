// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <TTFParser\Private\TTFParserAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

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
				static DynamicSizeAllocator meshGeneratorAllocator("Mesh Generator Allocator", RootAllocator::GetInstance());
				MeshGeneratorAllocator = &meshGeneratorAllocator;
			}
		}
	}
}