// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_SYSTEM_ALLOCATORS_H
#define RESOURCE_SYSTEM_ALLOCATORS_H

#include <MemoryManagement\Singleton.h>
#include <Allocators\DynamicSizeAllocator.h>
#include <Allocators\ThreadSafeAllocator.h>

namespace Engine
{
	using namespace Allocators;

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
				STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(ResourceAllocator);

			public:
				static ThreadSafeAllocator<DynamicSizeAllocator>* ResourceAllocator;
			};
		}
	}
}

#endif