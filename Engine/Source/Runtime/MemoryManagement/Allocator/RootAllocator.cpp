// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <MemoryManagement\Allocator\DefaultAllocator.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			SINGLETON_DEFINITION(RootAllocator)

			RootAllocator::RootAllocator(void) :
				DynamicSizeAllocator("Root Allocator", DefaultAllocator::GetInstance())
			{
			}
		}
	}
}