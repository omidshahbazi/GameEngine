// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>
#include <MemoryManagement\Allocator\MemoryHeader.h>
#include <Debugging\Debug.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			FixedSizeAllocator::FixedSizeAllocator(cstr Name, AllocatorBase *Parent, uint32 BlockSize, uint32 BlockCount) :
				CustomAllocator(Name, Parent, BlockCount * (GetHeaderSize() + BlockSize)),
				m_BlockSize(BlockSize)
			{
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