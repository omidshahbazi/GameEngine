// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\MemoryHandleAllocator.h>
#include <MemoryManagerment\MemoryHandle.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			MemoryHandleAllocator::MemoryHandleAllocator(uint32 ReserveCount) :
				m_ReserveCount(ReserveCount),
				m_FirstHandle(nullptr),
				m_HandlesStatus(nullptr)
			{
				m_FirstHandle = (MemoryHandle*)PlatformAllocate(ReserveCount * sizeof(MemoryHandle));
				m_HandlesStatus = (bool*)PlatformAllocate(ReserveCount);
				PlatformSet((byte*)m_HandlesStatus, 0, ReserveCount);
			}

			MemoryHandle *MemoryHandleAllocator::Allocate()
			{
				uint32 index = 0;
				for (; index <= m_ReserveCount; ++index)
					if (!m_HandlesStatus[index])
						break;

				m_HandlesStatus[index] = true;

				return (m_FirstHandle + index);
			}

			void MemoryHandleAllocator::Deallocate(MemoryHandle *Handle)
			{
				m_HandlesStatus[(Handle - m_FirstHandle)] = false;
			}
		}
	}
}