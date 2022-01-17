// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DEFAULT_ALLOCATOR_H
#define DEFAULT_ALLOCATOR_H

#include <Allocators\AllocatorBase.h>
#include <MemoryManagement\Singleton.h>

namespace Engine
{
	using namespace MemoryManagement;

	namespace Allocators
	{
		class ALLOCATORS_API DefaultAllocator : public AllocatorBase
		{
			CREATOR_DECLARATION(DefaultAllocator);
			GET_INSTANCE_DECLARATION(DefaultAllocator);


		private:
			DefaultAllocator(void);

#ifndef USE_VIRTUAL_ADDRESS_SPACE
			~DefaultAllocator(void);
#endif

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
		};
	}
}

#endif