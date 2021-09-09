// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_SYSTEM_ALLOCATORS_H
#define RENDER_SYSTEM_ALLOCATORS_H

#include <MemoryManagement\Singleton.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagement\Allocator\ThreadSafeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace RenderCommon
	{
		namespace Private
		{
			class RENDERCOMMON_API RenderSystemAllocators
			{
			public:
				CREATOR_DECLARATION(RenderSystemAllocators);

			private:
				RenderSystemAllocators(void);

			public:
				STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(RenderSystemAllocator);

				STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(ResourceAllocator);

				STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(ContainersAllocator);

				STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(ProgramCompilerAllocator);

			public:
				static DynamicSizeAllocator* RenderSystemAllocator;
				static ThreadSafeAllocator<DynamicSizeAllocator>* ResourceAllocator;
				static ThreadSafeAllocator<DynamicSizeAllocator>* ContainersAllocator;
				static DynamicSizeAllocator* ProgramCompilerAllocator;
			};
		}
	}
}

#endif