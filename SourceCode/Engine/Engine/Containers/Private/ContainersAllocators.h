// Copyright 2016-2020 ?????????????. All Rights Reserved.
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
			class CONTAINERS_API ContainersAllocators
			{
			public:
				DEFINE_ALLOCATOR_HELPERS(DynamicStringAllocator);
				DEFINE_ALLOCATOR_HELPERS(ConstStringAllocator);

			public:
				static DynamicSizeAllocator VectorAllocator;
				static DynamicSizeAllocator ListAllocator;
				static DynamicSizeAllocator MapAllocator;

			private:
				static DynamicSizeAllocator DynamicStringAllocator;
				static DynamicSizeAllocator ConstStringAllocator;
			};
		}
	}
}

#endif