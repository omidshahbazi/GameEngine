// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\DefaultAllocator.h>
#include <MemoryManagerment\MemoryHandle.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			DefaultAllocator *DefaultAllocator::instance = nullptr;

			MemoryHandle *DefaultAllocator::Allocate(uint32 Size)
			{
				return AllocateMemoryHandle(this, PlatformAllocate(Size), Size);
			}

			void DefaultAllocator::Deallocate(MemoryHandle *Handle)
			{
				PlatformDeallocate(Handle->Get());

				DeallocateMemoryHandle(Handle);
			}
		}
	}
}