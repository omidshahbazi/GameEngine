// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Containers\Private\ContainersAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace Containers
	{
		namespace Private
		{
			DynamicSizeAllocator ContainersAllocators::DynamicStringAllocator("DynamicString Allocator", RootAllocator::GetInstance(), MegaByte);
			DynamicSizeAllocator ContainersAllocators::ConstStringAllocator("ConstString Allocator", RootAllocator::GetInstance(), MegaByte);
			DynamicSizeAllocator ContainersAllocators::MapAllocator("Map Allocator", RootAllocator::GetInstance(), MegaByte);
		}
	}
}