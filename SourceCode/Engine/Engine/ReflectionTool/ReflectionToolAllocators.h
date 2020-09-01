// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef REFLECTION_TOOL_ALLOCATORS_H
#define REFLECTION_TOOL_ALLOCATORS_H
#include <MemoryManagement\Singleton.h>
#include <MemoryManagement\Allocator\DefaultAllocator.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace ReflectionTool
	{
		class ReflectionToolAllocators
		{
		public:
			CREATOR_DECLARATION(ReflectionToolAllocators);

		private:
			ReflectionToolAllocators(void);

		public:
			static DynamicSizeAllocator* TypesAllocator;
			DEFINE_STATIC_ALLOCATOR_HELPERS(TypesAllocator);
		};
	}
}
#endif