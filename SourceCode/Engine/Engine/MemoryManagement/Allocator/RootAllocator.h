// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef ROOT_ALLOCATOR_H
#define ROOT_ALLOCATOR_H

#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagement\Singleton.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class MEMORYMANAGEMENT_API RootAllocator : public DynamicSizeAllocator
			{
				SINGLETON_DEFINITION(RootAllocator)

			private:
				RootAllocator(void);
			};
		}
	}
}

#endif