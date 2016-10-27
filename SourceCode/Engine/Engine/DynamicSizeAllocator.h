// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include "Platform.h"
#include "MemoryHandle.h"

using namespace Engine::Platform;

namespace Engine::MemoryManagement::Allocator
{
#define TOTAL_RESERVED_MEMORY_SIZE 536870912

	class DynamicSizeAllocator
	{
	public:
		DynamicSizeAllocator() :
			m_LastHandleInfo(new HandleInfo(Memory::Allocate(TOTAL_RESERVED_MEMORY_SIZE), TOTAL_RESERVED_MEMORY_SIZE, true))
		{
		}

	public:
		template <typename T> MemoryHandle<T> Allocate(void)
		{
			unsigned int desireSize = sizeof(T);
			m_LastHandleInfo->Size -= desireSize;
			HandleInfo *newHandle = new HandleInfo(m_LastHandleInfo->Address, desireSize, false, m_LastHandleInfo->Previous, m_LastHandleInfo);
			if (m_LastHandleInfo->Previous != NULL)
				m_LastHandleInfo->Previous->Next = newHandle;
			m_LastHandleInfo->Address = (char*)m_LastHandleInfo->Address + desireSize;
			m_LastHandleInfo->Previous = newHandle;
			return MemoryHandle<T>(newHandle);
		}

		template <typename T> void Deallocate(const MemoryHandle<T> &Handle) const
		{
			Handle.m_Info->IsFree = true;
		}

		void Update(void)
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
				unsigned int totalSize = 0;
				nextHandle = handle;
				while ((nextHandle = nextHandle->Next) != nullptr)
				{
					nextHandle->Address = (char*)nextHandle->Address - handle->Size;
					if (nextHandle->IsFree)
					{
						nextHandle->Size += handle->Size;
						break;
					}
					totalSize += nextHandle->Size;
				}
				if (totalSize == 0)
					break;
				Memory::Copy((char*)handle->Address + handle->Size, handle->Address, totalSize);
				handle->Next->Previous = handle->Previous;
				if (handle->Previous != nullptr)
					handle->Previous->Next = handle->Next;
				delete handle;
				handle = nextHandle;
			} while ((handle = handle->Next) != nullptr);
		}

	private:
		HandleInfo *GetFirstHandle(void) const
		{
			HandleInfo *firstHandle = m_LastHandleInfo;
			while (firstHandle->Previous != nullptr)
				firstHandle = firstHandle->Previous;
			return firstHandle;
		}

	private:
		HandleInfo *m_LastHandleInfo;
	};
}