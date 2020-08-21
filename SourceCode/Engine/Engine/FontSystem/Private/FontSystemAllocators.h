// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FONT_SYSTEM_ALLOCATORS_H
#define FONT_SYSTEM_ALLOCATORS_H

#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace FontSystem
	{
		namespace Private
		{
			class FONTSYSTEM_API FontSystemAllocators
			{
			public:
				static DynamicSizeAllocator FontSystemAllocator;
				DEFINE_ALLOCATOR_HELPERS(FontSystemAllocator);
			};
		}
	}
}

#endif