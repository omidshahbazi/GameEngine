// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Allocators\FixedSizeAllocator.h>
#include <Allocators\Private\MemoryHeader.h>
#include <Common\PrimitiveTypes.h>

namespace Engine
{
	namespace Allocators
	{
		using namespace Private;

		FixedSizeAllocator::FixedSizeAllocator(cstr Name, AllocatorBase* Parent, uint32 BlockSize, uint32 BlockCount) :
			CustomAllocator(Name, Parent, BlockCount * BlockSize),
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

#ifndef ONLY_USING_C_ALLOCATOR
		MemoryHeader* FixedSizeAllocator::FindBestFitHeader(MemoryHeader* FirstFreeHeader, uint64 Size)
		{
			if (Size != m_BlockSize)
				return nullptr;

			HardAssert(FirstFreeHeader != nullptr, "Header cannot be null");

			return FirstFreeHeader;
		}
#endif
	}
}