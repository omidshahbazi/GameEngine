// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CUSTOM_ALLOCATOR_BASE_H
#define CUSTOM_ALLOCATOR_BASE_H

#include <Allocators\AllocatorBase.h>
#include <sstream>

namespace Engine
{
	using namespace Common;

	namespace Allocators
	{
		struct MemoryHeader;

		class ALLOCATORS_API CustomAllocator : public AllocatorBase
		{
		protected:
			CustomAllocator(cstr Name, AllocatorBase* Parent, uint64 ReserveSize = 0);
			virtual ~CustomAllocator(void);

		public:
#ifdef DEBUG_MODE
			virtual byte* Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function) override;
#else
			virtual byte* Allocate(uint64 Size) override;
#endif

#ifdef DEBUG_MODE
			virtual byte* Reallocate(byte* Address, uint64 Size, cstr File, uint32 LineNumber, cstr Function) override;
#else
			virtual byte* Reallocate(byte* Address, uint64 Size) override;
#endif
			virtual void Deallocate(byte* Address) override;
			virtual bool TryDeallocate(byte* Address) override;

#ifdef DEBUG_MODE
			void CheckForLeak(void);
#endif

			virtual uint64 GetReservedSize(void) const override
			{
				return m_ReservedSize;
			}

		protected:
#ifdef DEBUG_MODE
			virtual byte* AllocateInternal(uint64 Size, cstr File, uint32 LineNumber, cstr Function);
#else
			virtual byte* AllocateInternal(uint64 Size);
#endif

			virtual void Deallocate(MemoryHeader* Header);

			virtual MemoryHeader* InitializeHeader(byte* Address, uint64 Size);

			virtual void FreeHeader(MemoryHeader* Header, MemoryHeader* LastFreeHeader);
			virtual void ReallocateHeader(MemoryHeader* Header);

			virtual MemoryHeader* FindBestFitHeader(MemoryHeader* LastFreeHeader, uint64 Size) = 0;

			virtual MemoryHeader* GetHeaderFromAddress(byte* Address);
			virtual byte* GetAddressFromHeader(MemoryHeader* Header);

			virtual uint32 GetHeaderSize(void);

			virtual void RemoveHeaderFromList(MemoryHeader* Header);

#ifdef DEBUG_MODE
			virtual void SetDebugInfo(MemoryHeader* Header, cstr File, uint32 LineNumber, cstr Function);

			virtual void CheckCorruption(MemoryHeader* Header);

			static void CheckForCircularLink(MemoryHeader* Header);
#endif

			void PrintMemoryInfo(std::stringstream& Stream, MemoryHeader* Header, uint8 ValueLimit = 100);

			AllocatorBase* GetParent(void)
			{
				return m_Parent;
			}

		protected:
			AllocatorBase* m_Parent;
			uint64 m_ReservedSize;
			byte* m_StartAddress;
			byte* m_EndAddress;
			byte* m_LastFreeAddress;
			MemoryHeader* m_LastFreeHeader;

#ifndef ONLY_USING_C_ALLOCATOR
			uint64 m_TotalAllocated;
#endif

#ifdef DEBUG_MODE
			MemoryHeader* m_LastAllocatedHeader;
#endif
		};
	}
}

#endif