// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagement\Allocator\CustomAllocator.h>

#ifndef DYNAMIC_SIZE_ALLOCATOR_H
#define DYNAMIC_SIZE_ALLOCATOR_H

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class MEMORYMANAGEMENT_API DynamicSizeAllocator : public CustomAllocator
			{
			public:
				DynamicSizeAllocator(cstr Name, AllocatorBase *Parent, uint32 ReserveSize);

			protected:
				byte *GetFromFreeList(MemoryHeader *Header, uint64 Size = 0) override;
			};
		}
	}
}

#endif