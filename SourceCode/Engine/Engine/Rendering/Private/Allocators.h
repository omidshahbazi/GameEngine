// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>

#ifndef ALLOCATORS_H
#define ALLOCATORS_H

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
			};
		}
	}
}

#endif