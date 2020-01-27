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

				template<typename T>
				static T *Allocate(uint32 Count)
				{
					return ReinterpretCast(T*, AllocateMemory(&ResourceAllocator, Count * sizeof(T)));
				}

				template<typename T>
				static void Deallocate(T *Buffer)
				{
					DeallocateMemory(&ResourceAllocator, Buffer);
				}
			};
		}
	}
}

#endif