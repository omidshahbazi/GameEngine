// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\Private\ResourceAssetParserAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace ResourceAssetParser
	{
		namespace Private
		{
			DynamicSizeAllocator ResourceAssetParserAllocators::MeshhAllocator("Mesh Generator Allocator", RootAllocator::GetInstance(), 10 * MegaByte);
		}
	}
}