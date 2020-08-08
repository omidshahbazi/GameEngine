// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagement\Allocator\MemoryHeader.h>
#include <Debugging\Debug.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			DynamicSizeAllocator::DynamicSizeAllocator(cstr Name, AllocatorBase* Parent, uint64 ReserveSize) :
				CustomAllocator(Name, Parent, ReserveSize)
			{
			}

			byte* DynamicSizeAllocator::GetFromFreeList(MemoryHeader* Header, uint64 Size)
			{
				Assert(Header != nullptr, "Header cannot be null");
				Assert(Size != 0, "Size must be positive");

				MemoryHeader* bestFitHeader = nullptr;

				MemoryHeader* header = Header;
				while (header != nullptr)
				{
					if (header->Size >= Size && (bestFitHeader == nullptr || header->Size < bestFitHeader->Size))
						bestFitHeader = header;

					header = header->Previous;
				}

				if (bestFitHeader != nullptr)
				{
					ReallocateHeader(bestFitHeader);

					return GetAddressFromHeader(bestFitHeader);
				}

				return nullptr;
			}
		}
	}
}