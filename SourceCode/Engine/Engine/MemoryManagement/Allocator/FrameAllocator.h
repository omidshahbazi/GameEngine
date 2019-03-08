// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef FRAME_ALLOCATOR_H
#define FRAME_ALLOCATOR_H

#include <MemoryManagement\Allocator\CustomAllocator.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class MEMORYMANAGEMENT_API FrameAllocator : public CustomAllocator
			{
			public:
				FrameAllocator(cstr Name, AllocatorBase *Parent, uint64 ReserveSize);

			public:
#ifdef DEBUG_MODE
				virtual byte *Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function) override;
#else
				virtual byte *Allocate(uint64 Size) override;
#endif
				virtual void Deallocate(byte *Address) override;

				virtual void Reset(void);

			private:
				byte * GetFromFreeList(MemoryHeader *LastFreeHeader, uint64 Size) override
				{
					return nullptr;
				}
			};
		}
	}
}

#endif