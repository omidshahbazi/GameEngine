// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef ALLOCATORS_H
#define ALLOCATORS_H

#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace Parallelizing
	{
		namespace Private
		{
			class PARALLELIZING_API Allocators
			{
			public:
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