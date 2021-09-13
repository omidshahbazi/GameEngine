// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CORE_SYSTEM_ALLOCATORS_H
#define CORE_SYSTEM_ALLOCATORS_H

#include <MemoryManagement\Singleton.h>
#include <Allocators\DynamicSizeAllocator.h>

namespace Engine
{
	using namespace Allocators;

	namespace CoreSystem
	{
		namespace Private
		{
			class CORESYSTEM_API CoreSystemAllocators
			{
			public:
				CREATOR_DECLARATION(CoreSystemAllocators);

			private:
				CoreSystemAllocators(void);

			public:
				STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(CoreSystemAllocator);

			public:
				static DynamicSizeAllocator* CoreSystemAllocator;
			};
		}
	}
}

#endif