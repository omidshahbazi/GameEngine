// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Allocators\DefaultAllocator.h>
#include <Allocators\Initializer.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Platform;

	using namespace MemoryManagement;

	namespace Allocators
	{
		CREATOR_DEFINITION(DefaultAllocator);

#ifdef DEBUG_MODE
		byte* DefaultAllocator::Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
		byte* DefaultAllocator::Allocate(uint64 Size)
#endif
		{
			return PlatformMemory::Allocate(Size);
		}

#ifdef DEBUG_MODE
		byte* DefaultAllocator::Reallocate(byte* Address, uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
		byte* DefaultAllocator::Reallocate(byte* Address, uint64 Size)
#endif
		{
			return PlatformMemory::Reallocate(Address, Size);
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

		uint64 DefaultAllocator::GetReservedSize(void) const
		{
			return Initializer::GetInstance()->GetReservedSize();
		}
	}
}