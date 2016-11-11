// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\DefaultAllocator.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			DefaultAllocator *DefaultAllocator::instance = nullptr;

			byte *DefaultAllocator::Allocate(uint64 Size)
			{
				return PlatformMemory::Allocate(Size);
			}

			void DefaultAllocator::Deallocate(byte *Address)
			{
				PlatformMemory::Free(Address);
			}
		}
	}
}