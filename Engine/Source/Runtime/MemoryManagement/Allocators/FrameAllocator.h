// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FRAME_ALLOCATOR_H
#define FRAME_ALLOCATOR_H

#include <Allocators\CustomAllocator.h>

namespace Engine
{
	namespace Allocators
	{
		class ALLOCATORS_API FrameAllocator : public CustomAllocator
		{
		public:
			FrameAllocator(cstr Name, AllocatorBase* Parent, uint64 ReserveSize);

		public:
#ifndef ONLY_USING_C_ALLOCATOR
#ifdef DEBUG_MODE
			virtual byte* Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function) override;
#else
			virtual byte* Allocate(uint64 Size) override;
#endif

#ifdef DEBUG_MODE
			virtual byte* Reallocate(byte* Address, uint64 Size, cstr File, uint32 LineNumber, cstr Function) override;
#else
			virtual byte* Reallocate(byte* Address, uint64 Size) override;
#endif

			virtual void Deallocate(byte* Address) override
			{
			}

			virtual bool TryDeallocate(byte* Address) override
			{
				return true;
			}
#endif

			virtual void Reset(void);

#ifndef ONLY_USING_C_ALLOCATOR
		private:
			MemoryHeader* FindBestFitHeader(MemoryHeader* FirstFreeHeader, uint64 Size = 0) override
			{
				return nullptr;
			}
#endif
		};
	}
}

#endif