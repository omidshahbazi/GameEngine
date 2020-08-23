// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef REFLECTION_TOOL_ALLOCATORS_H
#define REFLECTION_TOOL_ALLOCATORS_H
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