// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERING_ALLOCATORS_H
#define RENDERING_ALLOCATORS_H

#include <MemoryManagement\Singleton.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagement\Allocator\FrameAllocator.h>
#include <Rendering\RenderingCommon.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace Rendering
	{
		namespace Private
		{
			class RENDERING_API RenderingAllocators
			{
			public:
				CREATOR_DECLARATION(RenderingAllocators);

			private:
				RenderingAllocators(void);

			public:
				STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(RenderingSystemAllocator);

				STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(ResourceAllocator);

				STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(ContainersAllocator);

				STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(ProgramCompilerAllocator);

			public:
				static DynamicSizeAllocator* RenderingSystemAllocator;
				static ThreadSafeAllocator<DynamicSizeAllocator>* ResourceAllocator;
				static ThreadSafeAllocator<DynamicSizeAllocator>* ContainersAllocator;
				static DynamicSizeAllocator* ProgramCompilerAllocator;
			};
		}
	}
}

#endif