// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PARALLELIZING_ALLOCATORS_H
#define PARALLELIZING_ALLOCATORS_H

#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace Parallelizing
	{
		namespace Private
		{
			class PARALLELIZING_API ParallelizingAllocators
			{
			public:
				DEFINE_ALLOCATOR_HELPERS(JobAllocator);
				DEFINE_ALLOCATOR_HELPERS(ThreadAllocator);
				DEFINE_ALLOCATOR_HELPERS(FiberAllocator);
				DEFINE_ALLOCATOR_HELPERS(ThreadWorkerArgumentsAllocator);
				DEFINE_ALLOCATOR_HELPERS(MainFiberWorkerArgumentAllocator);
				DEFINE_ALLOCATOR_HELPERS(TaskFiberWorkerArgumentAllocator);

			private:
				static DynamicSizeAllocator JobSystemAllocator;
				static DynamicSizeAllocator JobAllocator;
				static FixedSizeAllocator ThreadAllocator;
				static FixedSizeAllocator FiberAllocator;
				static FixedSizeAllocator ThreadWorkerArgumentsAllocator;
				static FixedSizeAllocator MainFiberWorkerArgumentAllocator;
				static FixedSizeAllocator TaskFiberWorkerArgumentAllocator;
			};
		}
	}
}

#endif