// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\HandleInfoAllocator.h>
#include <MemoryManagerment\HandleInfo.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			HandleInfoAllocator::HandleInfoAllocator(uint32 ReserveCount) :
				m_ReserveCount(ReserveCount),
				m_FirstHandle(nullptr),
				m_HandlesStatus(nullptr)
			{
				m_FirstHandle = (HandleInfo*)PlatformAllocate(ReserveCount * sizeof(HandleInfo));
				m_HandlesStatus = (bool*)PlatformAllocate(ReserveCount);
				PlatformSet((byte*)m_HandlesStatus, 0, ReserveCount);
			}

			HandleInfo *HandleInfoAllocator::Allocate()
			{
				uint32 index = 0;
				for (; index <= m_ReserveCount; ++index)
					if (!m_HandlesStatus[index])
						break;

				m_HandlesStatus[index] = true;

				return (m_FirstHandle + index);
			}

			void HandleInfoAllocator::Deallocate(HandleInfo *Handle)
			{
				m_HandlesStatus[(Handle - m_FirstHandle)] = false;
			}
		}
	}
}