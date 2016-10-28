// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\DefaultAllocator.h>
#include <MemoryManagerment\HandleInfo.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			DefaultAllocator *DefaultAllocator::instance = nullptr;

			HandleInfo *DefaultAllocator::Allocate(uint32 Size)
			{
				return AllocateHandleInfo(this, PlatformAllocate(Size), Size, false);
			}

			void DefaultAllocator::Deallocate(HandleInfo *Handle)
			{
				PlatformDeallocate(Handle->Get());

				DeallocateHandleInfo(Handle);
			}
		}
	}
}