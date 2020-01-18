// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef ALLOCATOR_BASE_H
#define ALLOCATOR_BASE_H

#include <Common\PrimitiveTypes.h>
#include <Debugging\Debug.h>

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

#ifdef DEBUG_MODE
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

			template<class Type>
			INLINE void DeallocateMemory(AllocatorBase &Allocator, Type *Pointer)
			{
				Allocator.Deallocate(ReinterpretCast(byte*, Pointer));
			}

			template<class Type>
			INLINE void DeallocateMemory(AllocatorBase *Allocator, Type *Pointer)
			{
				DeallocateMemory(*Allocator, Pointer);
			}

			template<class Type>
			INLINE void Construct(Type *Pointer)
			{
				new (Pointer) Type;
			}

			template<class Type, class... ValueType>
			INLINE void Construct(Type *Pointer, ValueType && ...Value)
			{
				new (Pointer) Type(std::forward<ValueType>(Value)...);
			}

#ifdef DEBUG_MODE
#define AllocateMemory(Allocator, Amount) \
	(Allocator)->Allocate(Amount, DEBUG_ARGUMENTS)
#else
#define AllocateMemory(Allocator, Amount) \
	(Allocator)->Allocate(Amount)
#endif

			const uint16 KiloByte = 1024;
			const uint32 MegaByte = 1024 * KiloByte;
			const uint64 GigaByte = 1024 * MegaByte;
		}
	}
}

#endif