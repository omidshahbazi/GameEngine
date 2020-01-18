// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Parallelizing\Private\ParallelizingAllocators.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Threading\Thread.h>
#include <Threading\Fiber.h>
#include <Parallelizing\Private\MainFiberWorkerArguments.h>
#include <Parallelizing\Private\ThreadWorkerArguments.h>
#include <Parallelizing\Private\TaskFiberWorkerArguments.h>

namespace Engine
{
	namespace Parallelizing
	{
		namespace Private
		{
			DynamicSizeAllocator ParallelizingAllocators::JobSystemAllocator("Job System Allocator", RootAllocator::GetInstance(), 10 * MegaByte);
			DynamicSizeAllocator ParallelizingAllocators::JobAllocator("Job Allocator", &JobSystemAllocator, 2 * MegaByte);
			FixedSizeAllocator ParallelizingAllocators::ThreadAllocator("Thread Allocator", &JobSystemAllocator, sizeof(Thread), 32);
			FixedSizeAllocator ParallelizingAllocators::FiberAllocator("Fiber Allocator", &JobSystemAllocator, sizeof(Fiber), 1000);
			FixedSizeAllocator ParallelizingAllocators::ThreadWorkerArgumentsAllocator("Thread Worker Argument Allocator", &JobSystemAllocator, sizeof(ThreadWorkerArguments), 32);
			FixedSizeAllocator ParallelizingAllocators::MainFiberWorkerArgumentAllocator("Main Fiber Worker Arguments Allocator", &JobSystemAllocator, sizeof(MainFiberWorkerArguments), 32);
			FixedSizeAllocator ParallelizingAllocators::TaskFiberWorkerArgumentAllocator("Task Fiber Worker Arguments Allocator", &JobSystemAllocator, sizeof(TaskFiberWorkerArguments), 1000);
		}
	}
}