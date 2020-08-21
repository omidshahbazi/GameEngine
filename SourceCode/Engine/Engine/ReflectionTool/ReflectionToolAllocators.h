// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef ALLOCATORS_H
#define ALLOCATORS_H
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace ReflectionTool
	{
		class ReflectionToolAllocators
		{
		public:
			static DynamicSizeAllocator TypesAllocator;
			DEFINE_ALLOCATOR_HELPERS(TypesAllocator);
		};
	}
}
#endif