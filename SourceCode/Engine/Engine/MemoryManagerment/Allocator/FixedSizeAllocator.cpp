// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\FixedSizeAllocator.h>
#include <MemoryManagerment\MemoryHandle.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			struct HandleStatus
			{
			public:
				bool Status;
				MemoryHandle *Handle;
			};

			const uint16 HANDLE_STATUS_SIZE = sizeof(HandleStatus);

			HandleStatus *handleStatus = nullptr;

			FixedSizeAllocator::FixedSizeAllocator(uint32 BlockSize, uint32 BlockCount) :
				m_BlockSize(BlockSize),
				m_BlockCount(BlockCount),
				m_Memory(nullptr),
				m_LastFreeIndex(0)
			{
				m_Memory = GetFromPool(BlockCount * BlockSize);
				handleStatus = (HandleStatus*)GetFromPool(BlockCount * HANDLE_STATUS_SIZE);
			}

			MemoryHandle *FixedSizeAllocator::Allocate(void)
			{
				MemoryHandle *handle = AllocateMemoryHandle(this, &m_Memory[m_LastFreeIndex * m_BlockSize], m_BlockSize);

				HandleStatus *status = &handleStatus[m_LastFreeIndex];

				status->Status = true;
				status->Handle = handle;

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