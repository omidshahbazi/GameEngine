// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONTAINERS_ALLOCATORS_H
#define CONTAINERS_ALLOCATORS_H

#include <MemoryManagement\Singleton.h>
#include <MemoryManagement\Allocator\DefaultAllocator.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagement\Allocator\ThreadSafeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace Containers
	{
		namespace Private
		{
			class CONTAINERS_API ContainersAllocators
			{
			public:
				CREATOR_DECLARATION(ContainersAllocators);

			private:
				ContainersAllocators(void);

			public:
				DEFINE_STATIC_DYNAMIC_ALLOCATOR_HELPERS(DynamicStringAllocator);
				DEFINE_STATIC_DYNAMIC_ALLOCATOR_HELPERS(ConstStringAllocator);

			public:
				static ThreadSafeAllocator<DynamicSizeAllocator>* DynamicStringAllocator;
				static ThreadSafeAllocator<DynamicSizeAllocator>* VectorAllocator;
				static ThreadSafeAllocator<DynamicSizeAllocator>* ListAllocator;
				static ThreadSafeAllocator<DynamicSizeAllocator>* StackAllocator;
				static ThreadSafeAllocator<DynamicSizeAllocator>* QueueAllocator;
				static ThreadSafeAllocator<DynamicSizeAllocator>* MapAllocator;

			private:
				static ThreadSafeAllocator<DynamicSizeAllocator>* ConstStringAllocator;
			};
		}
	}
}

#endif