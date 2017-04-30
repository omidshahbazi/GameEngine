// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>

#ifndef ROOT_ALLOCATOR_H
#define ROOT_ALLOCATOR_H

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class MEMORYMANAGEMENT_API RootAllocator : public DynamicSizeAllocator
			{
			private:
				RootAllocator(void);

				~RootAllocator(void)
				{
					delete instance;
				}

			public:
				static RootAllocator &GetInstance(void);

			private:
				static RootAllocator *instance;
			};
		}
	}
}

#endif