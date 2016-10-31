// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\DefaultAllocator.h>
#include <MemoryManagerment\MemoryHandle.h>
#include <Platform\Memory.h>

namespace Engine
{
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			DefaultAllocator *DefaultAllocator::instance = nullptr;

			MemoryHandle *DefaultAllocator::Allocate(uint32 Size)
			{
				return AllocateMemoryHandle(this, Memory::Allocate(Size), Size);
			}

			void DefaultAllocator::Deallocate(MemoryHandle *Handle)
			{
				Memory::Free(Handle->Get());

				DeallocateMemoryHandle(Handle);
			}
		}
	}
}