// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CUSTOM_ALLOCATOR_BASE_H
#define CUSTOM_ALLOCATOR_BASE_H

#include <MemoryManagement\Allocator\AllocatorBase.h>

namespace Engine
{
	using namespace Common;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			struct MemoryHeader;

			class MEMORYMANAGEMENT_API CustomAllocator : public AllocatorBase
			{
			protected:
				CustomAllocator(cstr Name, AllocatorBase* Parent, uint64 ReserveSize);
				virtual ~CustomAllocator(void);

			public:
#ifdef DEBUG_MODE
				virtual byte* Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function) override;
#else
				virtual byte* Allocate(uint64 Size) override;
#endif
				virtual void Deallocate(byte* Address) override;
				virtual bool TryDeallocate(byte* Address) override;

#ifdef DEBUG_MODE
				void CheckForLeak(void);
#endif

			protected:
				virtual void Deallocate(MemoryHeader* Header);

#ifdef DEBUG_MODE
				virtual MemoryHeader* InitializeHeader(byte* Address, uint64 Size, cstr File, uint32 LineNumber, cstr Function);
#else
				virtual MemoryHeader* InitializeHeader(byte* Address, uint64 Size);
#endif

				virtual void FreeHeader(MemoryHeader* Header, MemoryHeader* LastFreeHeader);
				virtual void ReallocateHeader(MemoryHeader* Header);

				virtual MemoryHeader* FindBestFitHeader(MemoryHeader* LastFreeHeader, uint64 Size) = 0;

				virtual MemoryHeader* GetHeaderFromAddress(byte* Address);
				virtual byte* GetAddressFromHeader(MemoryHeader* Header);

				virtual uint32 GetHeaderSize(void);

#ifdef DEBUG_MODE
				virtual void CheckCorruption(MemoryHeader* Header);

				virtual void CheckForDuplicate(MemoryHeader* Header, MemoryHeader* LastFreeHeader);
#endif

				AllocatorBase* GetParent(void)
				{
					return m_Parent;
				}

				void PrintMemoryInfo(MemoryHeader* Header, uint8 ValueLimit = 100);

			protected:
				AllocatorBase* m_Parent;
				uint32 m_ReserveSize;
				byte* m_StartAddress;
				byte* m_EndAddress;
				byte* m_LastFreeAddress;
				MemoryHeader* m_LastFreeHeader;

#ifndef ONLY_USING_C_ALLOCATOR
				uint32 m_TotalAllocated;
#endif

#ifdef DEBUG_MODE
				MemoryHeader* m_LastAllocatedHeader;
#endif
			};
		}
	}
}

#endif