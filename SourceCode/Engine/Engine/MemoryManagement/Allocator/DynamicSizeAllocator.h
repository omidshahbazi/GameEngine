// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DYNAMIC_SIZE_ALLOCATOR_H
#define DYNAMIC_SIZE_ALLOCATOR_H

#include <MemoryManagement\Allocator\CustomAllocator.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class MEMORYMANAGEMENT_API DynamicSizeAllocator : public CustomAllocator
			{
			public:
				DynamicSizeAllocator(cstr Name, AllocatorBase* Parent, uint64 ReserveSize);

			protected:
				MemoryHeader* FindBestFitHeader(MemoryHeader* Header, uint64 Size = 0) override;
			};
		}
	}
}

#endif