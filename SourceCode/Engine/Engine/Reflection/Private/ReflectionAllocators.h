// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef REFLECTION_ALLOCATORS_H
#define REFLECTION_ALLOCATORS_H

#include <MemoryManagement\Singleton.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace Reflection
	{
		namespace Private
		{
			class REFLECTION_API ReflectionAllocators
			{
			public:
				CREATOR_DECLARATION(ReflectionAllocators);

			private:
				ReflectionAllocators(void);

			public:
				STATIC_DYNAMIC_ALLOCATOR_HELPERS_DEFINITION(AutoGeneratedAllocator);

			public:
				static DynamicSizeAllocator* AutoGeneratedAllocator;
			};
		}
	}
}

#endif