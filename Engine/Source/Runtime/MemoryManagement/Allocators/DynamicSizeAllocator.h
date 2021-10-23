// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DYNAMIC_SIZE_ALLOCATOR_H
#define DYNAMIC_SIZE_ALLOCATOR_H

#include <Allocators\CustomAllocator.h>

namespace Engine
{
	namespace Allocators
	{
		class ALLOCATORS_API DynamicSizeAllocator : public CustomAllocator
		{
		public:
			DynamicSizeAllocator(cstr Name, AllocatorBase* Parent, uint64 ReserveSize);

		protected:
#ifndef ONLY_USING_C_ALLOCATOR
			MemoryHeader* FindBestFitHeader(MemoryHeader* FirstFreeHeader, uint64 Size = 0) override;
#endif
		};
	}
}

#endif