// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONTAINERS_ALLOCATORS_H
#define CONTAINERS_ALLOCATORS_H

#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace Containers
	{
		namespace Private
		{
			class CONTAINERS_API Allocators
			{
			public:
				static DynamicSizeAllocator DynamicStringAllocator;
				static DynamicSizeAllocator ConstStringAllocator;
			};
		}
	}
}

#endif