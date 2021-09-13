// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef REFLECTION_TOOL_ALLOCATORS_H
#define REFLECTION_TOOL_ALLOCATORS_H
#include <MemoryManagement\Singleton.h>
#include <Allocators\DefaultAllocator.h>
#include <Allocators\DynamicSizeAllocator.h>

namespace Engine
{
	using namespace Allocators;

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
			STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(TypesAllocator);
		};
	}
}
#endif