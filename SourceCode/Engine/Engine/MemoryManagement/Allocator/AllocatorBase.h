// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>
#include <Debugging\Debug.h>

#ifndef ALLOCATOR_BASE_H
#define ALLOCATOR_BASE_H

namespace Engine
{
	using namespace Common;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			class MEMORYMANAGEMENT_API AllocatorBase
			{
			public:
				AllocatorBase(cstr Name);

#if DEBUG_MODE
				virtual byte *Allocate(uint64 Amount, cstr File, uint32 LineNumber, cstr Function) = 0;
#else
				virtual byte *Allocate(uint64 Amount) = 0;
#endif

				virtual void Deallocate(byte *Address) = 0;

			protected:
				void PlatformCopy(const byte *Source, byte *Destination, uint64 Size);
				void PlatformSet(byte *Address, int32 Value, uint64 Size);

				INLINE cstr GetName(void) const
				{
					return m_Name;
				}

			private:
				cstr m_Name;
			};

#if DEBUG_MODE

#define AllocateMemory(Allocator, Amount) \
	(Allocator)->Allocate(Amount, DEBUG_ARGUMENTS)

#else

#define AllocateMemory(Allocator, Amount) \
	(Allocator)->Allocate(Amount)

#endif

#define DeallocateMemory(Allocator, Pointer) \
	(Allocator)->Deallocate(reinterpret_cast<byte*>(Pointer))

		}
	}
}

#endif