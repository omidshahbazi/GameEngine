// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef INSTANTIATION_H
#define INSTANTIATION_H

#include <MemoryManagement\Allocator\DefaultAllocator.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			namespace Private
			{
				class CAllocator : public AllocatorBase
				{
				public:
					CAllocator(void) :
						AllocatorBase("C Alloctor")
					{
					}
#ifdef DEBUG_MODE
					virtual byte* Allocate(uint64 Size, cstr File, uint32 LineNumber, cstr Function) override
#else
					virtual byte* Allocate(uint64 Size) override
#endif
					{
						return PlatformMemory::Allocate(Size);
					}

					void Deallocate(byte* Address) override
					{
						PlatformMemory::Free(Address);
					}

					bool TryDeallocate(byte* Address) override
					{
						PlatformMemory::Free(Address);

						return true;
					}
				};

				static CAllocator cAllocator;
				static const DefaultAllocator* defaultAllocator = DefaultAllocator::Create(&cAllocator);
				static const RootAllocator* rootAllocator = RootAllocator::Create(&cAllocator);
			}
		}
	}
}

#endif