// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\AllocatorBase.h>
#include <MemoryManagerment\Allocator\HandleInfoAllocator.h>
#include <MemoryManagerment\HandleInfo.h>
#include <Platform\Memory.h>
#include <new>

namespace Engine
{
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			HandleInfoAllocator HandleAllocator(1000000);

			HandleInfo *AllocatorBase::AllocateHandleInfo(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size, bool IsFree)
			{
				HandleInfo *handle = HandleAllocator.Allocate();

				new (handle) HandleInfo(OwnerAllocator, Address, Size, IsFree);

				return handle;
			}

			HandleInfo *AllocatorBase::AllocateHandleInfo(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size, bool IsFree, HandleInfo *Previous, HandleInfo *Next)
			{
				HandleInfo *handle = HandleAllocator.Allocate();

				new (handle) HandleInfo(OwnerAllocator, Address, Size, IsFree, Previous, Next);

				return handle;
			}

			void AllocatorBase::DeallocateHandleInfo(HandleInfo *Handle)
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