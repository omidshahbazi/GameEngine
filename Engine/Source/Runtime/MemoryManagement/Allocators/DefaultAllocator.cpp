// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Allocators\DefaultAllocator.h>
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
#ifdef USE_VIRTUAL_ADDRESS_SPACE
			byte* address = PlatformMemory::VirtualAllocate(Size);

			HardAssert(address != nullptr, "Couldn't allocate virtual memory");

			PlatformMemory::VirtualCommit(address, Size);

			return address;
#else
			return PlatformMemory::Allocate(Size);
#endif
		}

#ifdef DEBUG_MODE
		byte* DefaultAllocator::Reallocate(byte* Address, uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
		byte* DefaultAllocator::Reallocate(byte* Address, uint64 Size)
#endif
		{
#ifdef USE_VIRTUAL_ADDRESS_SPACE
			byte* address = PlatformMemory::VirtualAllocate(Size);
			PlatformMemory::VirtualCommit(address, Size);
			PlatformMemory::Copy(Address, address, Size);
			PlatformMemory::VirtualFree(Address);

			return address;
#else
			return PlatformMemory::Reallocate(Address, Size);
#endif
		}

		void DefaultAllocator::Deallocate(byte* Address)
		{
#ifdef USE_VIRTUAL_ADDRESS_SPACE
			PlatformMemory::VirtualFree(Address);
#else
			PlatformMemory::Free(Address);
#endif
		}

		bool DefaultAllocator::TryDeallocate(byte* Address)
		{
#ifdef USE_VIRTUAL_ADDRESS_SPACE
			PlatformMemory::VirtualFree(Address);
#else
			PlatformMemory::Free(Address);
#endif

			return true;
		}
	}
}