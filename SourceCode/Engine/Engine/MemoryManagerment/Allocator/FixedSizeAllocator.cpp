// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\FixedSizeAllocator.h>
#include <MemoryManagerment\MemoryHandle.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			FixedSizeAllocator::FixedSizeAllocator(uint32 BlockSize, uint32 BlockCount) :
				m_BlockSize(BlockSize),
				m_BlockCount(BlockCount),
				m_Memory(nullptr),
				m_LastFree(0)
			{
				m_Memory = PlatformAllocate(BlockCount * BlockSize);
			}

			MemoryHandle *FixedSizeAllocator::Allocate(void)
			{
				return AllocateMemoryHandle(this, &m_Memory[m_LastFree * m_BlockSize], m_BlockSize);
			}

			void FixedSizeAllocator::Deallocate(MemoryHandle *Handle)
			{
				DeallocateMemoryHandle(Handle);
			}

			void FixedSizeAllocator::Update(void)
			{
			}
		}
	}
}