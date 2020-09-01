// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <FontSystem\Private\FontSystemAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace FontSystem
	{
		namespace Private
		{
			CREATOR_DEFINITION(FontSystemAllocators);

			DynamicSizeAllocator* FontSystemAllocators::FontSystemAllocator = nullptr;

			FontSystemAllocators::FontSystemAllocators(void)
			{
				static DynamicSizeAllocator fontSystemAllocator("Font System Allocator", RootAllocator::GetInstance(), 500 * MegaByte);
				FontSystemAllocator = &fontSystemAllocator;
			}
		}
	}
}