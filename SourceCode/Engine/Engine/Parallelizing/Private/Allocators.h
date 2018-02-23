// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>

#ifndef ALLOCATORS_H
#define ALLOCATORS_H

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
			};
		}
	}
}

#endif