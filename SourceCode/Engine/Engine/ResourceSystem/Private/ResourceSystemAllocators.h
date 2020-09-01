// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_SYSTEM_ALLOCATORS_H
#define RESOURCE_SYSTEM_ALLOCATORS_H

#include <MemoryManagement\Singleton.h>
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
				CREATOR_DECLARATION(ResourceSystemAllocators);

			private:
				ResourceSystemAllocators(void);

			public:
				DEFINE_STATIC_ALLOCATOR_HELPERS1(ResourceAllocator);

			public:
				static DynamicSizeAllocator* ResourceAllocator;
			};
		}
	}
}

#endif