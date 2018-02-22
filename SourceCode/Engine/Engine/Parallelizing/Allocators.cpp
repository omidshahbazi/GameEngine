// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Parallelizing\Allocators.h>
#include <Threading\Thread.h>
#include <Threading\Fiber.h>

namespace Engine
{
	using namespace Threading;

	namespace Parallelizing
	{
		DynamicSizeAllocator Allocators::JobSystemAllocator("Job System Allocator", RootAllocator::GetInstance(), 1024 * 1024);
		DynamicSizeAllocator Allocators::JobAllocator("Job Allocator", &JobSystemAllocator, 2048);
	}
}