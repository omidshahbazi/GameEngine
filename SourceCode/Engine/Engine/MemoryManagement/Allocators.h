// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagement\Allocator\AllocatorBase.h>

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

namespace Engine
{
	namespace MemoryManagement
	{
		using namespace Allocator;

		class Allocators
		{
		public:
			static AllocatorBase &GetDynamicStringAllocator(void);

			static AllocatorBase &GetThreadAllocator(void);
			static AllocatorBase &GetThreadWorkerArgumentAllocator(void);
			static AllocatorBase &GetFiberAllocator(void);
			static AllocatorBase &GetMainFiberWorkerArgumentAllocator(void);
			static AllocatorBase &GetJobAllocator(void);
			static AllocatorBase &GetJobDescriptionAllocator(void);
		};
	}
}

#endif