// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagerment\Allocator\CustomAllocator.h>

#ifndef FIXED_SIZE_ALLOCATOR_H
#define FIXED_SIZE_ALLOCATOR_H

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class MEMORYMANAGERMENT_API FixedSizeAllocator : public CustomAllocator
			{
			public:
				FixedSizeAllocator(uint32 BlockSize, uint32 BlockCount);

			public:
				byte *Allocate(uint64 Amount) override;
				void Deallocate(byte *Handle) override;

			protected:
				byte *GetFromFreeList(MemoryHeader *Header, uint64 Size = 0) override;

			private:
				uint32 m_BlockSize;
			};
		}
	}
}

#endif