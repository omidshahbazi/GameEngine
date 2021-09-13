// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FONT_SYSTEM_ALLOCATORS_H
#define FONT_SYSTEM_ALLOCATORS_H

#include <MemoryManagement\Singleton.h>
#include <Allocators\DynamicSizeAllocator.h>

namespace Engine
{
	using namespace Allocators;

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
				STATIC_DYNAMIC_SIZE_ALLOCATOR_HELPERS_DEFINITION(FontSystemAllocator);

			public:
				static DynamicSizeAllocator* FontSystemAllocator;
			};
		}
	}
}

#endif