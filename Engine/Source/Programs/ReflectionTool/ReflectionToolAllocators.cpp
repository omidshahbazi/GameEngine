// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ReflectionTool\ReflectionToolAllocators.h>
#include <Allocators\RootAllocator.h>

namespace Engine
{
	namespace ReflectionTool
	{
		CREATOR_DEFINITION(ReflectionToolAllocators);

		DynamicSizeAllocator* ReflectionToolAllocators::TypesAllocator = nullptr;

		ReflectionToolAllocators::ReflectionToolAllocators(void)
		{
			static DynamicSizeAllocator typesAllocator("Types Allocator", RootAllocator::GetInstance(), 10 * MegaByte);
			TypesAllocator = &typesAllocator;

		}
	}
}
