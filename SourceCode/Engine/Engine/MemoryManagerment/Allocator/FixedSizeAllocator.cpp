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
				m_LastFreeIndex(0)
			{
				m_Memory = GetFromPool(BlockCount * BlockSize);
			}

			MemoryHandle *FixedSizeAllocator::Allocate(void)
			{
				MemoryHandle *handle = AllocateMemoryHandle(this, &m_Memory[m_LastFreeIndex * m_BlockSize], m_BlockSize);

				m_LastFreeIndex++;

				return handle;
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