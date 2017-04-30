// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagement\Allocator\AllocatorBase.h>

#ifndef DEFAULT_ALLOCATOR_H
#define DEFAULT_ALLOCATOR_H

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class MEMORYMANAGEMENT_API DefaultAllocator : public AllocatorBase
			{
			private:
				DefaultAllocator(void) :
					AllocatorBase("C Allocator")
				{
				}

				~DefaultAllocator(void)
				{
					delete instance;
				}

			public:
#if DEBUG_MODE
				virtual byte *Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function) override;
#else
				virtual byte *Allocate(uint64 Size) override;
#endif

				void Deallocate(byte *Address) override;

				static DefaultAllocator &GetInstance(void);

			private:
				static DefaultAllocator *instance;
			};
		}
	}
}

#endif