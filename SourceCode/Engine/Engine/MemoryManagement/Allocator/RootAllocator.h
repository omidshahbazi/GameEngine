// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagement\Singleton.h>

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
				SINGLETON_DEFINITION(RootAllocator)

			private:
				RootAllocator(void);
			};
		}
	}
}

#endif