// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagerment\MemoryHandle.h>
#include <Platform\Memory.h>

namespace Engine
{
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
#define TOTAL_RESERVED_MEMORY_SIZE 536870912

			DynamicSizeAllocator::DynamicSizeAllocator() :
				m_LastHandleInfo(new HandleInfo(this, Memory::Allocate(TOTAL_RESERVED_MEMORY_SIZE), TOTAL_RESERVED_MEMORY_SIZE, true))
			{
			}

			HandleInfo *DynamicSizeAllocator::Allocate(uint32 Size)
			{
				uint32 desireSize = Size;
				m_LastHandleInfo->Size -= desireSize;
				HandleInfo *newHandle = new HandleInfo(this, m_LastHandleInfo->Address, desireSize, false, m_LastHandleInfo->Previous, m_LastHandleInfo);
				if (m_LastHandleInfo->Previous != nullptr)
					m_LastHandleInfo->Previous->Next = newHandle;
				m_LastHandleInfo->Address = m_LastHandleInfo->Address + desireSize;
				m_LastHandleInfo->Previous = newHandle;
				return newHandle;
			}

			void DynamicSizeAllocator::Deallocate(HandleInfo *Handle)
			{
				//if (Handle->)

				Handle->IsFree = true;
			}

			void DynamicSizeAllocator::Update(void)
			{
				HandleInfo *handle = GetFirstHandle();
				HandleInfo *nextHandle = nullptr;
				do
				{
					if (!handle->IsFree)
						continue;
					nextHandle = handle;
					while ((nextHandle = handle->Next) != nullptr && nextHandle->IsFree)
					{
						handle->Size += nextHandle->Size;
						handle->Next = nextHandle->Next;
						if (nextHandle == m_LastHandleInfo)
							m_LastHandleInfo = handle;
						if (handle->Next != nullptr)
							handle->Next->Previous = handle;
						delete nextHandle;
					}
				} while ((handle = handle->Next) != nullptr);
				handle = GetFirstHandle();
				do
				{
					if (!handle->IsFree)
						continue;
					uint32 totalSize = 0;
					nextHandle = handle;
					while ((nextHandle = nextHandle->Next) != nullptr)
					{
						nextHandle->Address = nextHandle->Address - handle->Size;
						if (nextHandle->IsFree)
						{
							nextHandle->Size += handle->Size;
							break;
						}
						totalSize += nextHandle->Size;
					}
					if (totalSize == 0)
						break;
					Memory::Copy(handle->Address + handle->Size, handle->Address, totalSize);
					handle->Next->Previous = handle->Previous;
					if (handle->Previous != nullptr)
						handle->Previous->Next = handle->Next;
					delete handle;
					handle = nextHandle;
				} while ((handle = handle->Next) != nullptr);
			}

			HandleInfo *DynamicSizeAllocator::GetFirstHandle(void) const
			{
				HandleInfo *firstHandle = m_LastHandleInfo;
				while (firstHandle->Previous != nullptr)
					firstHandle = firstHandle->Previous;
				return firstHandle;
			}
		}
	}
}