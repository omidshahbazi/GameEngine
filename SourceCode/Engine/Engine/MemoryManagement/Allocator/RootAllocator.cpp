// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <MemoryManagement\Allocator\DefaultAllocator.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			const uint64 SIZE = 10240 * 1024 * 1024;

			RootAllocator *RootAllocator::instance = nullptr;

			RootAllocator::RootAllocator(void) :
				DynamicSizeAllocator("Root Allocator", &DefaultAllocator::GetInstance(), SIZE)
			{
			}

			RootAllocator &RootAllocator::GetInstance(void)
			{
				if (instance == nullptr)
					instance = new RootAllocator;

				return *instance;
			}
		}
	}
}