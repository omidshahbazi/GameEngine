// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagement\Allocator\AllocatorBase.h>

#ifndef CUSTOM_ALLOCATOR_BASE_H
#define CUSTOM_ALLOCATOR_BASE_H

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
				CustomAllocator(cstr Name, AllocatorBase *Parent, uint64 ReserveSize);
				virtual ~CustomAllocator(void);

			public:
#if DEBUG_MODE
				virtual byte *Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function) override;
#else
				virtual byte *Allocate(uint64 Size) override;
#endif
				void Deallocate(byte *Address) override;

			protected:
#if DEBUG_MODE
				virtual void InitializeHeader(byte *Address, uint64 Size, cstr File, uint32 LineNumber, cstr Function);
#else
				virtual void InitializeHeader(byte *Address, uint64 Size);
#endif

				virtual void FreeHeader(MemoryHeader *Header, MemoryHeader *LastFreeHeader);
				virtual void ReallocateHeader(MemoryHeader *Header);

				virtual byte *GetFromFreeList(MemoryHeader *LastFreeHeader, uint64 Size) = 0;

				virtual MemoryHeader *GetHeaderFromAddress(byte *Address);
				virtual byte *GetAddressFromHeader(MemoryHeader *Extra);

				virtual uint32 GetHeaderSize(void);

				AllocatorBase *GetParent(void)
				{
					return m_Parent;
				}

			private:
				AllocatorBase *m_Parent;
				uint32 m_ReserveSize;
				byte *m_StartAddress;
				byte *m_EndAddress;
				byte *m_LastFreeAddress;
				MemoryHeader *m_LastFreeHeader;

#if DEBUG_MODE
				MemoryHeader *m_LastAllocatedHeader;
#endif
			};
		}
	}
}

#endif