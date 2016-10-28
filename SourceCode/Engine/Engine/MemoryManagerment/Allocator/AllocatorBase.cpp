// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\AllocatorBase.h>
#include <MemoryManagerment\Allocator\MemoryHandleAllocator.h>
#include <MemoryManagerment\MemoryHandle.h>
#include <Platform\Memory.h>
#include <new>

namespace Engine
{
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			MemoryHandleAllocator HandleAllocator(1000000);

			MemoryHandle *AllocatorBase::AllocateMemoryHandle(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size)
			{
				MemoryHandle *handle = HandleAllocator.Allocate();

				new (handle) MemoryHandle(OwnerAllocator, Address, Size);

				return handle;
			}

			void AllocatorBase::DeallocateMemoryHandle(MemoryHandle *Handle)
			{
				HandleAllocator.Deallocate(Handle);
			}

			byte *AllocatorBase::PlatformAllocate(uint32 Size)
			{
				return Memory::Allocate(Size);
			}

			void AllocatorBase::PlatformDeallocate(byte *Address)
			{
				Memory::Free(Address);
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