// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Parallelizing\Private\Allocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	namespace Parallelizing
	{
		namespace Private
		{
			DynamicSizeAllocator Allocators::JobSystemAllocator("Job System Allocator", RootAllocator::GetInstance(), 1024 * 1024);
			DynamicSizeAllocator Allocators::JobAllocator("Job Allocator", &JobSystemAllocator, 2048);
		}
	}
}