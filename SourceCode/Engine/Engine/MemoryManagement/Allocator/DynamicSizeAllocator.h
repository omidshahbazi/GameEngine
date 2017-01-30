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
				DynamicSizeAllocator(uint32 ReserveSize);

			public:
				byte *Allocate(uint64 Amount) override;

				void Deallocate(byte *Address) override;

			protected:
				byte *GetFromFreeList(MemoryHeader *Header, uint64 Size = 0) override;
			};
		}
	}
}

#endif