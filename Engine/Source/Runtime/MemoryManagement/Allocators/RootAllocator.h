// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef ROOT_ALLOCATOR_H
#define ROOT_ALLOCATOR_H

#include <Allocators\DynamicSizeAllocator.h>
#include <MemoryManagement\Singleton.h>

namespace Engine
{
	using namespace MemoryManagement;

	namespace Allocators
	{
		class ALLOCATORS_API RootAllocator : public DynamicSizeAllocator
		{
			SINGLETON_DECLARATION(RootAllocator);

		private:
			RootAllocator(void);
		};
	}
}

#endif