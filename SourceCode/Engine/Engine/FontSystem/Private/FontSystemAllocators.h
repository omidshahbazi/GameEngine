// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FONT_SYSTEM_ALLOCATORS_H
#define FONT_SYSTEM_ALLOCATORS_H

#include <MemoryManagement\Singleton.h>
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
				CREATOR_DECLARATION(FontSystemAllocators);

			private:
				FontSystemAllocators(void);

			public:
				DEFINE_STATIC_ALLOCATOR_HELPERS(FontSystemAllocator);

			public:
				static DynamicSizeAllocator* FontSystemAllocator;
			};
		}
	}
}

#endif