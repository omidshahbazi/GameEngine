// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Allocators\DynamicSizeAllocator.h>
#include <Allocators\Private\MemoryHeader.h>
#include <Common\PrimitiveTypes.h>

namespace Engine
{
	namespace Allocators
	{
		using namespace Private;

		DynamicSizeAllocator::DynamicSizeAllocator(cstr Name, AllocatorBase* Parent, uint64 ReserveSize) :
			CustomAllocator(Name, Parent, ReserveSize)
		{
		}

#ifndef ONLY_USING_C_ALLOCATOR
		MemoryHeader* DynamicSizeAllocator::FindBestFitHeader(MemoryHeader* FirstFreeHeader, uint64 Size)
		{
			HardAssert(FirstFreeHeader != nullptr, "Header cannot be null");

			MemoryHeader* bestFitHeader = nullptr;

			MemoryHeader* header = FirstFreeHeader;
			while (header != nullptr)
			{
				if (header->Size >= Size && (bestFitHeader == nullptr || header->Size < bestFitHeader->Size))
				{
					bestFitHeader = header;

					if (bestFitHeader->Size == Size)
						break;
				}

				header = header->Next;
			}

			return bestFitHeader;
		}
#endif
	}
}