// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\FrameAllocator.h>
#include <MemoryManagement\Allocator\MemoryHeader.h>
#include <Platform\PlatformMemory.h>
#include <Debugging\Debug.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			FrameAllocator::FrameAllocator(cstr Name, AllocatorBase *Parent, uint64 ReserveSize) :
				CustomAllocator(Name, Parent, ReserveSize)
			{
			}

#ifdef DEBUG_MODE
			byte *FrameAllocator::Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function)
#else
			byte *FrameAllocator::Allocate(uint64 Size)
#endif
			{
#ifdef ONLY_USING_C_ALLOCATOR
				return Platform::PlatformMemory::Allocate(Size);
#else
				Assert(m_LastFreeAddress < m_EndAddress, "No more memory to allocate");

				byte *address = m_LastFreeAddress;

				m_LastFreeAddress += Size;

				Assert(m_LastFreeAddress <= m_EndAddress, "End of the block is out of allocator's bound");

				return address;

#endif
			}

			void FrameAllocator::Deallocate(byte *Address)
			{
			}

			void FrameAllocator::Reset(void)
			{
				m_LastFreeAddress = m_StartAddress;
			}
		}
	}
}