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
			FrameAllocator(cstr Name, AllocatorBase* Parent, uint64 ReserveSize = 0);

		public:
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

			virtual void Deallocate(byte* Address) override;
			virtual bool TryDeallocate(byte* Address) override;

			virtual void Reset(void);

		private:
			MemoryHeader* FindBestFitHeader(MemoryHeader* Header, uint64 Size = 0) override
			{
				return nullptr;
			}
		};
	}
}

#endif