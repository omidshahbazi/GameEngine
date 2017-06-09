// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagement\Allocator\CustomAllocator.h>

#ifndef FIXED_SIZE_ALLOCATOR_H
#define FIXED_SIZE_ALLOCATOR_H

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class MEMORYMANAGEMENT_API FixedSizeAllocator : public CustomAllocator
			{
			public:
				FixedSizeAllocator(cstr Name, AllocatorBase *Parent, uint32 BlockSize, uint32 BlockCount);

#if DEBUG_MODE
				virtual byte *Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function) override;
#else
				virtual byte *Allocate(uint64 Size) override;
#endif

			protected:
				byte *GetFromFreeList(MemoryHeader *Header, uint64 Size = 0) override;

			private:
				uint32 m_BlockSize;
			};
		}
	}
}

#endif