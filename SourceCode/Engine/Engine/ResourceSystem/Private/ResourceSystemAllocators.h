// Copyright 2016-2020 ?????????????. All Rights Reserved.
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
				DEFINE_ALLOCATOR_HELPERS(ResourceAllocator);
			};
		}
	}
}

#endif