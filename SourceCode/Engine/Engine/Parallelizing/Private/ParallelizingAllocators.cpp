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

			ThreadSafeAllocator<DynamicSizeAllocator>* ParallelizingAllocators::JobSystemAllocator = nullptr;
			ThreadSafeAllocator<DynamicSizeAllocator>* ParallelizingAllocators::JobAllocator = nullptr;
			ThreadSafeAllocator<FixedSizeAllocator>* ParallelizingAllocators::ThreadAllocator = nullptr;
			ThreadSafeAllocator<FixedSizeAllocator>* ParallelizingAllocators::FiberAllocator = nullptr;
			ThreadSafeAllocator<FixedSizeAllocator>* ParallelizingAllocators::ThreadWorkerArgumentsAllocator = nullptr;
			ThreadSafeAllocator<FixedSizeAllocator>* ParallelizingAllocators::MainFiberWorkerArgumentAllocator = nullptr;
			ThreadSafeAllocator<FixedSizeAllocator>* ParallelizingAllocators::TaskFiberWorkerArgumentAllocator = nullptr;

			ParallelizingAllocators::ParallelizingAllocators(void)
			{
				static ThreadSafeAllocator<DynamicSizeAllocator> jobSystemAllocator("Job System Allocator", RootAllocator::GetInstance());
				JobSystemAllocator = &jobSystemAllocator;

				static ThreadSafeAllocator<DynamicSizeAllocator> jobAllocator("Job Allocator", &jobSystemAllocator);
				JobAllocator = &jobAllocator;

				static ThreadSafeAllocator<FixedSizeAllocator> threadAllocator("Thread Allocator", &jobSystemAllocator, sizeof(Thread));
				ThreadAllocator = &threadAllocator;

				static ThreadSafeAllocator<FixedSizeAllocator> fiberAllocator("Fiber Allocator", &jobSystemAllocator, sizeof(Fiber));
				FiberAllocator = &fiberAllocator;

				static ThreadSafeAllocator<FixedSizeAllocator> threadWorkerArgumentsAllocator("Thread Worker Argument Allocator", &jobSystemAllocator, sizeof(ThreadWorkerArguments));
				ThreadWorkerArgumentsAllocator = &threadWorkerArgumentsAllocator;

				static ThreadSafeAllocator<FixedSizeAllocator> mainFiberWorkerArgumentAllocator("Main Fiber Worker Arguments Allocator", &jobSystemAllocator, sizeof(MainFiberWorkerArguments));
				MainFiberWorkerArgumentAllocator = &mainFiberWorkerArgumentAllocator;

				static ThreadSafeAllocator<FixedSizeAllocator> taskFiberWorkerArgumentAllocator("Task Fiber Worker Arguments Allocator", &jobSystemAllocator, sizeof(TaskFiberWorkerArguments));
				TaskFiberWorkerArgumentAllocator = &taskFiberWorkerArgumentAllocator;
			}
		}
	}
}