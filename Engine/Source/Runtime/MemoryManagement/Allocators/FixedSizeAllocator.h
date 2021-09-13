// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FIXED_SIZE_ALLOCATOR_H
#define FIXED_SIZE_ALLOCATOR_H

#include <Allocators\CustomAllocator.h>

namespace Engine
{
	namespace Allocators
	{
		class ALLOCATORS_API FixedSizeAllocator : public CustomAllocator
		{
		public:
			FixedSizeAllocator(cstr Name, AllocatorBase* Parent, uint32 BlockSize, uint32 BlockCount = 0);

#ifdef DEBUG_MODE
			virtual byte* Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function) override;
#else
			virtual byte* Allocate(uint64 Size) override;
#endif

		protected:
			MemoryHeader* FindBestFitHeader(MemoryHeader* Header, uint64 Size = 0) override;

		private:
			uint32 m_BlockSize;
		};
	}
}

#endif