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
				DynamicSizeAllocator("Root Allocator", DefaultAllocator::GetInstance(), GigaByte * 3) //TODO: find a way to set this value before start for editor, launcher and tools
			{
			}
		}
	}
}