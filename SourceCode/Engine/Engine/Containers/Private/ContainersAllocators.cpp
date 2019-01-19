// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Containers\Private\ContainersAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace Containers
	{
		namespace Private
		{
			DynamicSizeAllocator ContainersAllocators::DynamicStringAllocator("DynamicString Allocator", RootAllocator::GetInstance(), MegaByte * 100);
			DynamicSizeAllocator ContainersAllocators::ConstStringAllocator("ConstString Allocator", RootAllocator::GetInstance(), MegaByte * 100);
			DynamicSizeAllocator ContainersAllocators::VectorAllocator("Vector Allocator", RootAllocator::GetInstance(), MegaByte * 100);
			DynamicSizeAllocator ContainersAllocators::MapAllocator("Map Allocator", RootAllocator::GetInstance(), MegaByte * 100);
		}
	}
}