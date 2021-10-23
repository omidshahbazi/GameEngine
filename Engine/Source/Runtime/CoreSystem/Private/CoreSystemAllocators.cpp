// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <CoreSystem\Private\CoreSystemAllocators.h>
#include <Allocators\RootAllocator.h>

namespace Engine
{
	namespace CoreSystem
	{
		namespace Private
		{
			CREATOR_DEFINITION(CoreSystemAllocators);

			DynamicSizeAllocator* CoreSystemAllocators::CoreSystemAllocator = nullptr;

			CoreSystemAllocators::CoreSystemAllocators(void)
			{
				static DynamicSizeAllocator coreSystemAllocator("Core System Allocator", RootAllocator::GetInstance(), 128 * MegaByte);
				CoreSystemAllocator = &coreSystemAllocator;
			}
		}
	}
}