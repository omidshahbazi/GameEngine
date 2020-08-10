// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>
#include <MemoryManagement\Allocator\MemoryHeader.h>
#include <Debugging\Debug.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			FixedSizeAllocator::FixedSizeAllocator(cstr Name, AllocatorBase* Parent, uint32 BlockSize, uint32 BlockCount) :
				CustomAllocator(Name, Parent, BlockCount* (GetHeaderSize() + BlockSize)),
				m_BlockSize(BlockSize)
			{
			}

#ifdef DEBUG_MODE
			byte* FixedSizeAllocator::Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
			byte* FixedSizeAllocator::Allocate(uint64 Size)
#endif
			{
#ifdef DEBUG_MODE
				return CustomAllocator::Allocate(Size * m_BlockSize, File, LineNumber, Function);
#else
				return CustomAllocator::Allocate(Size * m_BlockSize);
#endif
			}

			MemoryHeader* FixedSizeAllocator::FindBestFitHeader(MemoryHeader* Header, uint64 Size)
			{
				if (Size != m_BlockSize)
					return nullptr;

				Assert(Header != nullptr, "Header cannot be null");

				return Header;
			}
		}
	}
}