// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\DefaultAllocator.h>
#include <MemoryManagement\Allocator\Initializer.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			//SINGLETON_DEFINITION(DefaultAllocator);
			CREATOR_DEFINITION(DefaultAllocator);

#ifdef DEBUG_MODE
			byte* DefaultAllocator::Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
			byte* DefaultAllocator::Allocate(uint64 Size)
#endif
			{
				return PlatformMemory::Allocate(Size);
			}

			void DefaultAllocator::Deallocate(byte* Address)
			{
				PlatformMemory::Free(Address);
			}

			bool DefaultAllocator::TryDeallocate(byte* Address)
			{
				PlatformMemory::Free(Address);

				return true;
			}

			uint32 DefaultAllocator::GetReservedSize(void) const
			{
				return Initializer::GetInstance()->GetReservedSize();
			}
		}
	}
}