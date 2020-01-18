// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <CoreSystem\Private\CoreSystemAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace CoreSystem
	{
		namespace Private
		{
			DynamicSizeAllocator CoreSystemAllocators::CoreSystemAllocator("Core System Allocator", RootAllocator::GetInstance(), MegaByte);
		}
	}
}