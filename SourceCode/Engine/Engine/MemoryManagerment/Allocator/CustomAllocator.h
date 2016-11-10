// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagerment\Allocator\AllocatorBase.h>

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

			class MEMORYMANAGERMENT_API CustomAllocator : public AllocatorBase
			{
			protected:
				CustomAllocator(uint64 ReserveSize);

				virtual byte *AllocateInternal(uint64 Size);
				virtual void DeallocateInternal(byte *Address);

				virtual void InitializeHeader(byte *Address, uint64 Size);
				virtual void FreeHeader(MemoryHeader *Header, MemoryHeader *LastFreeHeader);
				virtual void ReallocateHeader(MemoryHeader *Header);

				virtual byte *GetFromFreeList(MemoryHeader *LastFreeHeader, uint64 Size) = 0;

				virtual MemoryHeader *GetHeaderFromAddress(byte *Address);
				virtual byte *GetAddressFromHeader(MemoryHeader *Extra);

				virtual uint32 GetHeaderSize(void);

			private:
				uint32 m_ReserveSize;
				byte *m_StartAddress;
				byte *m_EndAddress;
				byte *m_LastFreeAddress;
				MemoryHeader *m_LastFreeHeader;
			};
		}
	}
}

#endif