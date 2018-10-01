// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERING_ALLOCATORS_H
#define RENDERING_ALLOCATORS_H

#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace Rendering
	{
		namespace Private
		{
			class RENDERING_API Allocators
			{
			public:
				static DynamicSizeAllocator RenderingSystemAllocator;
				static DynamicSizeAllocator ShaderCompilerAllocator;
			};
		}
	}
}

#endif