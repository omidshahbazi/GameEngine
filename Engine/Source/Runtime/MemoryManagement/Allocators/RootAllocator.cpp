// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Allocators\RootAllocator.h>
#include <Allocators\DefaultAllocator.h>

namespace Engine
{
	using namespace MemoryManagement;

	namespace Allocators
	{
		SINGLETON_DEFINITION(RootAllocator);

		RootAllocator::RootAllocator(void) :
			DynamicSizeAllocator("Root Allocator", DefaultAllocator::GetInstance())
		{
		}
	}
}