// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\FrameAllocator.h>
#include <MemoryManagement\Allocator\MemoryHeader.h>
#include <Platform\PlatformMemory.h>
#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			FrameAllocator::FrameAllocator(cstr Name, AllocatorBase* Parent, uint64 ReserveSize) :
				CustomAllocator(Name, Parent, ReserveSize)
			{
			}

#ifdef DEBUG_MODE
			byte* FrameAllocator::Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
			byte* FrameAllocator::Allocate(uint64 Size)
#endif
			{
#ifdef ONLY_USING_C_ALLOCATOR
				return Platform::PlatformMemory::Allocate(Size);
#else
				HardAssert(Size != 0, "Allocating zero size is not applicable");
				HardAssert(m_LastFreeAddress < m_EndAddress, "No more memory to allocate");

				byte* address = m_LastFreeAddress;

				m_LastFreeAddress += Size;

				HardAssert(m_LastFreeAddress <= m_EndAddress, "End of the block is out of allocator's bound");

				return address;
#endif
			}

#ifdef DEBUG_MODE
			byte* FrameAllocator::Reallocate(byte* Address, uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
			byte* FrameAllocator::Reallocate(byte* Address, uint64 Size)
#endif
			{
				byte* newAddress = nullptr;

#ifdef DEBUG_MODE
				newAddress = Allocate(Size, File, LineNumber, Function);
#else
				newAddress = Allocate(uint64 Size);
#endif

				if (Address != nullptr)
					PlatformMemory::Copy(Address, newAddress, Size);

				return newAddress;
			}

			void FrameAllocator::Deallocate(byte* Address)
			{
			}

			bool FrameAllocator::TryDeallocate(byte* Address)
			{
				return true;
			}

			void FrameAllocator::Reset(void)
			{
				m_LastFreeAddress = m_StartAddress;
			}
		}
	}
}