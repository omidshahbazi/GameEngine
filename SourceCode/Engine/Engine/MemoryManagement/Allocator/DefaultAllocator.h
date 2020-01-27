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
				SINGLETON_DECLARATION(DefaultAllocator)

			private:
				DefaultAllocator(void) :
					AllocatorBase("C Allocator")
				{
				}

			public:
#ifdef DEBUG_MODE
				virtual byte *Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function) override;
#else
				virtual byte *Allocate(uint64 Size) override;
#endif

				void Deallocate(byte *Address) override;
			};
		}
	}
}

#endif