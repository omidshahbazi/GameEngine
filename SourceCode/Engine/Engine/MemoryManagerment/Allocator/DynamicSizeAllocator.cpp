// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagerment\MemoryHeader.h>
#include <Debugging\Debug.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			DynamicSizeAllocator::DynamicSizeAllocator(uint32 ReserveSize) :
				CustomAllocator(ReserveSize)
			{
			}

			byte *DynamicSizeAllocator::Allocate(uint64 Amount)
			{
				return AllocateInternal(Amount);
			}

			void DynamicSizeAllocator::Deallocate(byte *Address)
			{
				DeallocateInternal(Address);
			}

			byte *DynamicSizeAllocator::GetFromFreeList(MemoryHeader *Header, uint64 Size)
			{
				Assert(Header != nullptr, "Header cannot be null");
				Assert(Size != 0, "Size must be positive");

				MemoryHeader *header = Header;
				while (header != nullptr)
				{
					if (header->Size >= Size)
						break;

					header = header->Previous;
				}

				if (header != nullptr)
				{
					ReallocateHeader(header);

					return GetAddressFromHeader(header);
				}

				return nullptr;
			}
		}
	}
}