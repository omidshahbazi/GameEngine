// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\AllocatorBase.h>
#include <Platform\Memory.h>

//using namespace Engine::Platform;

#ifndef DEFAULT_ALLOCATOR_H
#define DEFAULT_ALLOCATOR_H

//namespace Engine::MemoryManagement::Allocator
//{
	class DefaultAllocator : AllocatorBase
	{
	public:
		byte *Allocate(uint32 Size) override
		{
			return Memory::Allocate(Size);
		}

		void Deallocate(byte *Address) override
		{
			Memory::Free(Address);
		}
	};
//}

#endif