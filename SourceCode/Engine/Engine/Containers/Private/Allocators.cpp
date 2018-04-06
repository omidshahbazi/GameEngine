// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Containers\Private\Allocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace Containers
	{
		namespace Private
		{
			DynamicSizeAllocator Allocators::DynamicStringAllocator("DynamicString Allocator", RootAllocator::GetInstance(), MegaByte);
			DynamicSizeAllocator Allocators::ConstStringAllocator("ConstString Allocator", RootAllocator::GetInstance(), MegaByte);
		}
	}
}