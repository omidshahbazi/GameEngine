// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Allocators\DefaultAllocator.h>
#include <Platform\PlatformMemory.h>
#include <Platform\PlatformOS.h>

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

			HardAssert(PlatformMemory::VirtualCommit(address, Size), "Couldn't commit allocated virtual memory");

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
			HardAssert(address != nullptr, "Couldn't allocate virtual memory");

			HardAssert(PlatformMemory::VirtualCommit(address, Size), "Couldn't commit allocated virtual memory");

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
			HardAssert(PlatformMemory::VirtualFree(Address), "Couldn't free up virtual memory");
#else
			PlatformMemory::Free(Address);
#endif
		}

		bool DefaultAllocator::TryDeallocate(byte* Address)
		{
#ifdef USE_VIRTUAL_ADDRESS_SPACE
			HardAssert(PlatformMemory::VirtualFree(Address), "Couldn't free up virtual memory");
#else
			PlatformMemory::Free(Address);
#endif

			return true;
		}
	}
}