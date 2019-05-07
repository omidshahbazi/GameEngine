// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\Private\FontSystemAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace FontSystem
	{
		namespace Private
		{
			DynamicSizeAllocator FontSystemAllocators::FontSystemAllocator("Font System Allocator", RootAllocator::GetInstance(), 500 * MegaByte);
		}
	}
}