// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\MemoryHandleAllocator.h>
#include <MemoryManagerment\MemoryHandle.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			const uint64 COUNT = 1000000;

			MemoryHandleAllocator::MemoryHandleAllocator(void) :
				m_FirstHandle(nullptr),
				m_HandlesStatus(nullptr)
			{
				m_FirstHandle = (MemoryHandle*)GetFromPool(COUNT * sizeof(MemoryHandle));
				m_HandlesStatus = (bool*)GetFromPool(COUNT);
				PlatformSet((byte*)m_HandlesStatus, 0, COUNT);
			}

			MemoryHandle *MemoryHandleAllocator::Allocate()
			{
				uint32 index = 0;
				for (; index <= COUNT; ++index)
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