// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ReflectionTool\Allocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace ReflectionTool
	{
		DynamicSizeAllocator Allocators::TypesAllocator("Types Allocator", RootAllocator::GetInstance(), 10 * MegaByte);
	}
}
