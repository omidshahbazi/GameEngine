// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <MemoryManagement\Allocator\DefaultAllocator.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			const uint64 SIZE = 1073741824;

			SINGLETON_DECLARATION(RootAllocator)

			RootAllocator::RootAllocator(void) :
				DynamicSizeAllocator("Root Allocator", DefaultAllocator::GetInstance(), SIZE)
			{
			}
		}
	}
}