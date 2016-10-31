// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagerment\Allocator\AllocatorBase.h>

#ifndef FIXED_SIZE_ALLOCATOR_H
#define FIXED_SIZE_ALLOCATOR_H

namespace Engine
{
	namespace MemoryManagement
	{
		class MemoryHandleExtra;

		namespace Allocator
		{
			class FixedSizeAllocator : public AllocatorBase
			{
			public:
				FixedSizeAllocator(uint32 BlockSize, uint32 BlockCount);

			public:
				MemoryHandle *Allocate(uint32 Count);

				void Deallocate(MemoryHandle *Handle) override;

				void Defragment(void);

			private:
				uint32 m_BlockSize;
				uint32 m_BlockCount;
				byte *m_Memory;
				uint32 m_LastFreeIndex;
				MemoryHandleExtra *m_FirstHandleExtra;
				MemoryHandleExtra *m_LastHandleExtra;
			};
		}
	}
}

#endif