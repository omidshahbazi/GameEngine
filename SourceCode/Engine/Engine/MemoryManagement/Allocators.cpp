// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagement\Allocators.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>

namespace Engine
{
	namespace MemoryManagement
	{
		using namespace Allocator;

		AllocatorBase &Allocators::GetDynamicStringAllocator(void)
		{
			static DynamicSizeAllocator allocator(10485760);

			return allocator;
		}

		AllocatorBase &Allocators::GetThreadAllocator(void)
		{
			static FixedSizeAllocator allocator(sizeof(Thread) + sizeof(JobFiberWorkerArguments), 100);

			return allocator;
		}

		AllocatorBase &Allocators::GetThreadWorkerArgumentAllocator(void)
		{
			static FixedSizeAllocator allocator(sizeof(ThreadWorkerArgument) + sizeof(JobFiberWorkerArguments), 100);

			return allocator;
		}

		AllocatorBase &Allocators::GetFiberAllocator(void)
		{
			static FixedSizeAllocator allocator(sizeof(Fiber) + sizeof(JobFiberWorkerArguments), 100);

			return allocator;
		}

		AllocatorBase &Allocators::GetMainFiberWorkerArgumentAllocator(void)
		{
			static FixedSizeAllocator allocator(sizeof(Fiber) + sizeof(JobFiberWorkerArguments), 100);

			return allocator;
		}

		AllocatorBase &Allocators::GetJobAllocator(void)
		{
			static FixedSizeAllocator allocator(sizeof(Job), 1000);

			return allocator;
		}

		AllocatorBase &Allocators::GetJobDescriptionAllocator(void)
		{
			static FixedSizeAllocator allocator(sizeof(JobDescription), 1000);

			return allocator;
		}
	}
}