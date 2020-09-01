// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\Private\ResourceAssetParserAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace ResourceAssetParser
	{
		namespace Private
		{
			CREATOR_DEFINITION(ResourceAssetParserAllocators);

			DynamicSizeAllocator* ResourceAssetParserAllocators::MeshGeneratorAllocator = nullptr;

			ResourceAssetParserAllocators::ResourceAssetParserAllocators(void)
			{
				static DynamicSizeAllocator meshGeneratorAllocator("Mesh Generator Allocator", RootAllocator::GetInstance(), 32 * MegaByte);
				MeshGeneratorAllocator = &meshGeneratorAllocator;
			}
		}
	}
}