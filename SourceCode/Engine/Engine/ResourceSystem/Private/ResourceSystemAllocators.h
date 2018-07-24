// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_SYSTEM_ALLOCATORS_H
#define RESOURCE_SYSTEM_ALLOCATORS_H

#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace ResourceSystem
	{
		namespace Private
		{
			class RESOURCESYSTEM_API ResourceSystemAllocators
			{
			public:
				static DynamicSizeAllocator ResourceAllocator;
			};
		}
	}
}

#endif