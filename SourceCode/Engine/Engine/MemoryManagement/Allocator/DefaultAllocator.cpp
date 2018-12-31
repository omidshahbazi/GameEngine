// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\DefaultAllocator.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			SINGLETON_DEFINITION(DefaultAllocator)

#if DEBUG_MODE
			byte *DefaultAllocator::Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
			byte *DefaultAllocator::Allocate(uint64 Size)
#endif
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