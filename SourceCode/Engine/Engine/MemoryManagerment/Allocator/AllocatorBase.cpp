// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\AllocatorBase.h>
#include <MemoryManagerment\Allocator\MemoryHandleAllocator.h>
#include <MemoryManagerment\MemoryHandle.h>
#include <MemoryManagerment\Allocator\Pool\MemoryPool.h>
#include <Platform\Memory.h>
#include <new>

namespace Engine
{
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			using namespace Pool;

			MemoryPool MEMORY_POOL;
			MemoryHandleAllocator HANDLE_ALLOCATOR;

			MemoryHandle *AllocatorBase::AllocateMemoryHandle(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size)
			{
				MemoryHandle *handle = HANDLE_ALLOCATOR.Allocate();

				new (handle) MemoryHandle(OwnerAllocator, Address, Size);

				return handle;
			}

			void AllocatorBase::DeallocateMemoryHandle(MemoryHandle *Handle)
			{
				Assert(Handle->GetOwnerAllocator() == this, "MemoryHandle from another allocator cannot deallocate with other allocator");

				HANDLE_ALLOCATOR.Deallocate(Handle);
			}

			byte *AllocatorBase::GetFromPool(uint32 Size)
			{
				return MEMORY_POOL.Get(Size);
			}

			void AllocatorBase::PlatformCopy(const byte *Source, byte *Destination, uint32 Size)
			{
				Memory::Copy(Source, Destination, Size);
			}

			void AllocatorBase::PlatformSet(byte *Address, int32 Value, uint32 Size)
			{
				Memory::Set(Address, Value, Size);
			}
		}
	}
}