// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace ResourceSystem
	{
		namespace Private
		{
			CREATOR_DEFINITION(ResourceSystemAllocators);

			DynamicSizeAllocator* ResourceSystemAllocators::ResourceAllocator = nullptr;

			ResourceSystemAllocators::ResourceSystemAllocators(void)
			{
				static DynamicSizeAllocator resourceAllocator("Resource Allocator", RootAllocator::GetInstance());
				ResourceAllocator = &resourceAllocator;
			}
		}
	}
}