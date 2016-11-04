// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\DefaultAllocator.h>
#include <Platform\Memory.h>

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
				return Memory::Allocate(Size);
			}

			void DefaultAllocator::Deallocate(byte *Address)
			{
				Memory::Free(Address);
			}
		}
	}
}