// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>

#ifndef ALLOCATOR_BASE_H
#define ALLOCATOR_BASE_H

namespace Engine
{
	using namespace Common;

	namespace MemoryManagement
	{
		class MemoryHandle;

		namespace Allocator
		{
			class AllocatorBase
			{
			public:
				virtual void Deallocate(MemoryHandle *Handle)
				{ }

			protected:
				MemoryHandle *AllocateMemoryHandle(AllocatorBase *OwnerAllocator, byte *Address, uint32 Size);
				void DeallocateMemoryHandle(MemoryHandle *Handle);

				byte *GetFromPool(uint32 Size);
				byte *PlatformAllocate(uint64 Size);
				void PlatformDeallocate(byte *Address);
				void PlatformCopy(const byte *Source, byte *Destination, uint64 Size);
				void PlatformSet(byte *Address, int32 Value, uint64 Size);
			};
		}
	}
}

#endif