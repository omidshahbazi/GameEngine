// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DEFAULT_ALLOCATOR_H
#define DEFAULT_ALLOCATOR_H

#include <MemoryManagement\Allocator\AllocatorBase.h>
#include <MemoryManagement\Singleton.h>

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class MEMORYMANAGEMENT_API DefaultAllocator : public AllocatorBase
			{
				CREATOR_DECLARATION(DefaultAllocator);
				GET_INSTANCE_DECLARATION(DefaultAllocator);


			private:
				DefaultAllocator(void) :
					AllocatorBase("Default Allocator")
				{
				}

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

				void Deallocate(byte* Address) override;
				bool TryDeallocate(byte* Address) override;

			protected:
				virtual uint64 GetReservedSize(void) const override;
			};
		}
	}
}

#endif