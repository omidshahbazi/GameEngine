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
				DEFINE_STATIC_ALLOCATOR_HELPERS(RenderingSystemAllocator);

				DEFINE_STATIC_ALLOCATOR_HELPERS(ShaderCompilerAllocator);

			public:
				static DynamicSizeAllocator* RenderingSystemAllocator;
				static DynamicSizeAllocator* ContainersAllocator;
				static DynamicSizeAllocator* ShaderCompilerAllocator;
				static FrameAllocator* CommandAllocators[(int8)RenderQueues::COUNT];
			};
		}
	}
}

#endif