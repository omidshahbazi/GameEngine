// Copyright 2016-2020 ?????????????. All Rights Reserved.
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
			CREATOR_DEFINITION(ParallelizingAllocators);

			DynamicSizeAllocator* ParallelizingAllocators::JobSystemAllocator = nullptr;
			DynamicSizeAllocator* ParallelizingAllocators::JobAllocator = nullptr;
			FixedSizeAllocator* ParallelizingAllocators::ThreadAllocator = nullptr;
			FixedSizeAllocator* ParallelizingAllocators::FiberAllocator = nullptr;
			FixedSizeAllocator* ParallelizingAllocators::ThreadWorkerArgumentsAllocator = nullptr;
			FixedSizeAllocator* ParallelizingAllocators::MainFiberWorkerArgumentAllocator = nullptr;
			FixedSizeAllocator* ParallelizingAllocators::TaskFiberWorkerArgumentAllocator = nullptr;

			ParallelizingAllocators::ParallelizingAllocators(void)
			{
				static DynamicSizeAllocator jobSystemAllocator("Job System Allocator", RootAllocator::GetInstance());
				JobSystemAllocator = &jobSystemAllocator;

				static DynamicSizeAllocator jobAllocator("Job Allocator", &jobSystemAllocator);
				JobAllocator = &jobAllocator;

				static FixedSizeAllocator threadAllocator("Thread Allocator", &jobSystemAllocator, sizeof(Thread));
				ThreadAllocator = &threadAllocator;

				static FixedSizeAllocator fiberAllocator("Fiber Allocator", &jobSystemAllocator, sizeof(Fiber));
				FiberAllocator = &fiberAllocator;

				static FixedSizeAllocator threadWorkerArgumentsAllocator("Thread Worker Argument Allocator", &jobSystemAllocator, sizeof(ThreadWorkerArguments));
				ThreadWorkerArgumentsAllocator = &threadWorkerArgumentsAllocator;

				static FixedSizeAllocator mainFiberWorkerArgumentAllocator("Main Fiber Worker Arguments Allocator", &jobSystemAllocator, sizeof(MainFiberWorkerArguments));
				MainFiberWorkerArgumentAllocator = &mainFiberWorkerArgumentAllocator;

				static FixedSizeAllocator taskFiberWorkerArgumentAllocator("Task Fiber Worker Arguments Allocator", &jobSystemAllocator, sizeof(TaskFiberWorkerArguments));
				TaskFiberWorkerArgumentAllocator = &taskFiberWorkerArgumentAllocator;
			}
		}
	}
}