// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <Allocators\RootAllocator.h>

namespace Engine
{
	namespace ResourceSystem
	{
		namespace Private
		{
			CREATOR_DEFINITION(ResourceSystemAllocators);

			ThreadSafeAllocator<DynamicSizeAllocator>* ResourceSystemAllocators::ResourceAllocator = nullptr;

			ResourceSystemAllocators::ResourceSystemAllocators(void)
			{
				static ThreadSafeAllocator<DynamicSizeAllocator> resourceAllocator("Resource Allocator", RootAllocator::GetInstance(), 1 * GigaByte);
				ResourceAllocator = &resourceAllocator;
			}
		}
	}
}