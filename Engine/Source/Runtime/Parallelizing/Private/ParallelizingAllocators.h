// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PARALLELIZING_ALLOCATORS_H
#define PARALLELIZING_ALLOCATORS_H

#include <MemoryManagement\Singleton.h>
#include <Allocators\ThreadSafeAllocator.h>
#include <Allocators\DynamicSizeAllocator.h>
#include <Allocators\FixedSizeAllocator.h>

namespace Engine
{
	using namespace Allocators;

	namespace Parallelizing
	{
		namespace Private
		{
			class PARALLELIZING_API ParallelizingAllocators
			{
			public:
				CREATOR_DECLARATION(ParallelizingAllocators);

			private:
				ParallelizingAllocators(void);

			public:
				STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(JobAllocator);
				STATIC_FIXED_SIZE_ALLOCATOR_HELPERS_DEFINITION(ThreadAllocator);
				STATIC_FIXED_SIZE_ALLOCATOR_HELPERS_DEFINITION(FiberAllocator);
				STATIC_FIXED_SIZE_ALLOCATOR_HELPERS_DEFINITION(TaskFiberWorkerArgumentAllocator);

			public:
				static ThreadSafeAllocator<FixedSizeAllocator>* FiberAllocator;
				static ThreadSafeAllocator<DynamicSizeAllocator>* WaitingListAllocator;

			private:
				static ThreadSafeAllocator<DynamicSizeAllocator>* JobSystemAllocator;
				static ThreadSafeAllocator<DynamicSizeAllocator>* JobAllocator;
				static ThreadSafeAllocator<FixedSizeAllocator>* ThreadAllocator;
				static ThreadSafeAllocator<FixedSizeAllocator>* TaskFiberWorkerArgumentAllocator;
			};
		}
	}
}

#endif