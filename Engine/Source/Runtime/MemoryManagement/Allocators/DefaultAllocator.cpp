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
			//byte* address = PlatformMemory::VirtualAllocate(Size);

			//HardAssert(address != nullptr, "Couldn't allocate virtual memory");

			//PlatformMemory::VirtualCommit(address, Size);

			//return address;

			return PlatformMemory::Allocate(Size);
		}

#ifdef DEBUG_MODE
		byte* DefaultAllocator::Reallocate(byte* Address, uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
		byte* DefaultAllocator::Reallocate(byte* Address, uint64 Size)
#endif
		{
			//byte* address = PlatformMemory::VirtualAllocate(Size);
			//PlatformMemory::VirtualCommit(address, Size);
			//PlatformMemory::Copy(Address, address, Size);
			//PlatformMemory::VirtualFree(Address);

			//return address;

			return PlatformMemory::Reallocate(Address, Size);
		}

		void DefaultAllocator::Deallocate(byte* Address)
		{
			//PlatformMemory::VirtualFree(Address);
			PlatformMemory::Free(Address);
		}

		bool DefaultAllocator::TryDeallocate(byte* Address)
		{
			//PlatformMemory::VirtualFree(Address);
			PlatformMemory::Free(Address);

			return true;
		}
	}
}