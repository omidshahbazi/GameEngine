// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\FixedSizeAllocator.h>
#include <MemoryManagerment\HandleInfo.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			FixedSizeAllocator::FixedSizeAllocator(uint32 BlockSize, uint32 BlockCount) :
				m_BlockSize(BlockSize),
				m_BlockCount(BlockCount),
				m_LastHandleInfo(nullptr)
			{
				m_LastHandleInfo = AllocateHandleInfo(this, PlatformAllocate(m_BlockSize * m_BlockCount), m_BlockSize * m_BlockCount, true);
			}

			HandleInfo *FixedSizeAllocator::Allocate(void)
			{
				//uint32 desireSize = Size;
				//m_LastHandleInfo->Size -= desireSize;
				//HandleInfo *newHandle = new HandleInfo(this, m_LastHandleInfo->Address, desireSize, false, m_LastHandleInfo->Previous, m_LastHandleInfo);
				//if (m_LastHandleInfo->Previous != nullptr)
				//	m_LastHandleInfo->Previous->Next = newHandle;
				//m_LastHandleInfo->Address = m_LastHandleInfo->Address + desireSize;
				//m_LastHandleInfo->Previous = newHandle;
				//return newHandle;
				return nullptr;
			}

			void FixedSizeAllocator::Deallocate(HandleInfo *Handle)
			{
				//if (Handle->)

				Handle->IsFree = true;
			}

			void FixedSizeAllocator::Update(void)
			{
			}
		}
	}
}