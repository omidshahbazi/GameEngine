// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CUSTOM_ALLOCATOR_H
#define CUSTOM_ALLOCATOR_H

#include <Allocators\AllocatorBase.h>
#include <sstream>

#ifndef DEBUG_MODE
#undef LEAK_DETECTION
#undef CORRUPTED_HEAP_DETECTION
#endif

#ifdef ONLY_USING_C_ALLOCATOR
#undef LEAK_DETECTION
#undef CORRUPTED_HEAP_DETECTION
#endif

namespace Engine
{
	using namespace Common;

	namespace Allocators
	{
		namespace Private
		{
			struct MemoryHeader;
		}

		using namespace Private;

		class ALLOCATORS_API CustomAllocator : public AllocatorBase
		{
		protected:
			CustomAllocator(cstr Name, AllocatorBase* Parent, uint64 ReserveSize);
			virtual ~CustomAllocator(void);

		public:
#ifdef DEBUG_MODE
			virtual byte* Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function) override
#else
			virtual byte* Allocate(uint64 Size) override
#endif
			{
#ifdef DEBUG_MODE
				return AllocateInternal(Size, File, LineNumber, Function);
#else
				return AllocateInternal(Size);
#endif
			}

#ifdef DEBUG_MODE
			virtual byte* Reallocate(byte* Address, uint64 Size, cstr File, uint32 LineNumber, cstr Function) override;
#else
			virtual byte* Reallocate(byte* Address, uint64 Size) override;
#endif
			virtual void Deallocate(byte* Address) override;
			virtual bool TryDeallocate(byte* Address) override;

			virtual uint64 GetReservedSize(void) const override
			{
#ifdef ONLY_USING_C_ALLOCATOR
				return 0;
#else
				return m_ReservedSize;
#endif
			}

		protected:
#ifdef DEBUG_MODE
			virtual byte* AllocateInternal(uint64 Size, cstr File, uint32 LineNumber, cstr Function);
#else
			virtual byte* AllocateInternal(uint64 Size);
#endif

#ifndef ONLY_USING_C_ALLOCATOR
			virtual void Deallocate(MemoryHeader* Header);

			virtual MemoryHeader* InitializeHeader(byte* Address, uint64 Size);

			virtual void FreeHeader(MemoryHeader* Header);

			virtual MemoryHeader* FindBestFitHeader(MemoryHeader* FirstFreeHeader, uint64 Size) = 0;

			virtual MemoryHeader* GetHeaderFromAddress(byte* Address);
			virtual byte* GetAddressFromHeader(MemoryHeader* Header);

			virtual uint32 GetHeaderSize(void);

			virtual void AddHeaderToChain(MemoryHeader* Header, MemoryHeader** FirstHeader);
			virtual void RemoveHeaderFromChain(MemoryHeader* Header, MemoryHeader** FirstHeader);

#ifdef DEBUG_MODE
			virtual void SetDebugInfo(MemoryHeader* Header, uint64 UserSize, cstr File, uint32 LineNumber, cstr Function);
#endif

#ifdef LEAK_DETECTION
			virtual void CheckLeakage(void);
#endif

#ifdef CORRUPTED_HEAP_DETECTION
			virtual void CheckCorruption(MemoryHeader* Header);
#endif

#ifdef DEBUG_MODE
			void PrintMemoryInfo(std::stringstream& Stream, MemoryHeader* Header, uint8 ValueLimit = 100);
#endif

			virtual void Reset(void);
#endif

			AllocatorBase* GetParent(void)
			{
				return m_Parent;
			}

		protected:
			AllocatorBase* m_Parent;

#ifndef ONLY_USING_C_ALLOCATOR
			uint64 m_ReservedSize;
			byte* m_StartAddress;
			byte* m_EndAddress;

			byte* m_LastFreeAddress;
			MemoryHeader* m_FirstFreeHeader;

			uint64 m_TotalAllocated;
#endif

#ifdef LEAK_DETECTION
			MemoryHeader* m_FirstAllocatedHeader;
#endif
		};
	}
}

#endif