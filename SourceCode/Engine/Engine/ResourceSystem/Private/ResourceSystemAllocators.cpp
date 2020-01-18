// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace ResourceSystem
	{
		namespace Private
		{
			DynamicSizeAllocator ResourceSystemAllocators::ResourceAllocator("Resource Allocator", RootAllocator::GetInstance(), 500 * MegaByte);
		}
	}
}