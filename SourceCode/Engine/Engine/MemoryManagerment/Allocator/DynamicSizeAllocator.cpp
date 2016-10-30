// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagerment\MemoryHandle.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			DynamicSizeAllocator::DynamicSizeAllocator(uint32 ReserveSize) :
				m_ReserveSize(ReserveSize),
				m_LastHandleInfo(nullptr)
			{
				m_LastHandleInfo = AllocateMemoryHandle(this, GetFromPool(m_ReserveSize), m_ReserveSize);
			}

			MemoryHandle *DynamicSizeAllocator::Allocate(uint32 Size)
			{
				//uint32 desireSize = Size;
				//m_LastHandleInfo->Size -= desireSize;
				//HandleInfo *newHandle = AllocateHandleInfo(this, m_LastHandleInfo->Address, desireSize, false, m_LastHandleInfo->Previous, m_LastHandleInfo);
				//if (m_LastHandleInfo->Previous != nullptr)
				//	m_LastHandleInfo->Previous->Next = newHandle;
				//m_LastHandleInfo->Address = m_LastHandleInfo->Address + desireSize;
				//m_LastHandleInfo->Previous = newHandle;
				//return newHandle;

				return nullptr;
			}

			void DynamicSizeAllocator::Deallocate(MemoryHandle *Handle)
			{
				//if (Handle->)

				//Handle->IsFree = true;
			}

			void DynamicSizeAllocator::Update(void)
			{
				//HandleInfo *handle = GetFirstHandle();
				//HandleInfo *nextHandle = nullptr;
				//do
				//{
				//	if (!handle->IsFree)
				//		continue;
				//	nextHandle = handle;
				//	while ((nextHandle = handle->Next) != nullptr && nextHandle->IsFree)
				//	{
				//		handle->Size += nextHandle->Size;
				//		handle->Next = nextHandle->Next;
				//		if (nextHandle == m_LastHandleInfo)
				//			m_LastHandleInfo = handle;
				//		if (handle->Next != nullptr)
				//			handle->Next->Previous = handle;
				//		DeallocateHandleInfo(nextHandle);
				//	}
				//} while ((handle = handle->Next) != nullptr);
				//handle = GetFirstHandle();
				//do
				//{
				//	if (!handle->IsFree)
				//		continue;
				//	uint32 totalSize = 0;
				//	nextHandle = handle;
				//	while ((nextHandle = nextHandle->Next) != nullptr)
				//	{
				//		nextHandle->Address = nextHandle->Address - handle->Size;
				//		if (nextHandle->IsFree)
				//		{
				//			nextHandle->Size += handle->Size;
				//			break;
				//		}
				//		totalSize += nextHandle->Size;
				//	}
				//	if (totalSize == 0)
				//		break;
				//	PlatformCopy(handle->Address + handle->Size, handle->Address, totalSize);
				//	handle->Next->Previous = handle->Previous;
				//	if (handle->Previous != nullptr)
				//		handle->Previous->Next = handle->Next;
				//	DeallocateHandleInfo(handle);
				//	handle = nextHandle;
				//} while ((handle = handle->Next) != nullptr);
			}

			MemoryHandle *DynamicSizeAllocator::GetFirstHandle(void) const
			{
				//HandleInfo *firstHandle = m_LastHandleInfo;
				//while (firstHandle->Previous != nullptr)
				//	firstHandle = firstHandle->Previous;
				//return firstHandle;

				return nullptr;
			}
		}
	}
}