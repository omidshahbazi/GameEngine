// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\FixedSizeAllocator.h>
#include <MemoryManagerment\MemoryHeader.h>
#include <Debugging\Debug.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			FixedSizeAllocator::FixedSizeAllocator(uint32 BlockSize, uint32 BlockCount) :
				CustomAllocator(BlockCount * (GetHeaderSize() + BlockSize)),
				m_BlockSize(BlockSize)
			{
			}

			byte *FixedSizeAllocator::Allocate(uint64 Amount)
			{
				return AllocateInternal(Amount * m_BlockSize);
			}

			void FixedSizeAllocator::Deallocate(byte *Address)
			{
				DeallocateInternal(Address);
			}

			byte *FixedSizeAllocator::GetFromFreeList(MemoryHeader *Header, uint64 Size)
			{
				if (Size != m_BlockSize)
					return nullptr;

				Assert(Header != nullptr, "Header cannot be null");

				ReallocateHeader(Header);

				return GetAddressFromHeader(Header);
			}
		}
	}
}