// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\MemoryHandleAllocator.h>
#include <MemoryManagerment\MemoryHandle.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			const uint16 HANDLE_SIZE = sizeof(MemoryHandle) + sizeof(MemoryHandleExtra);

			MemoryHandleAllocator::MemoryHandleAllocator(uint32 ReserveCount) :
				m_ReserveCount(ReserveCount),
				m_Memory(nullptr),
				m_HandlesStatus(nullptr)
			{
				m_Memory = GetFromPool(ReserveCount * HANDLE_SIZE);
				m_HandlesStatus = (bool*)GetFromPool(ReserveCount);
				PlatformSet((byte*)m_HandlesStatus, 0, ReserveCount);
			}

			MemoryHandle *MemoryHandleAllocator::Allocate(void)
			{
				uint32 index = 0;
				for (; index <= m_ReserveCount; ++index)
					if (!m_HandlesStatus[index])
						break;

				m_HandlesStatus[index] = true;

				return (MemoryHandle*)(m_Memory + (index * HANDLE_SIZE));
			}

			void MemoryHandleAllocator::Deallocate(MemoryHandle *Handle)
			{
				uint32 index = (byte*)Handle - m_Memory;

				m_HandlesStatus[index / HANDLE_SIZE] = false;
			}
		}
	}
}