// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Platform\Memory.h>

//using namespace Engine::Platform;

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

//namespace Engine::MemoryManagement::Allocator
//{
	class DefaultAllocator
	{
	public:
		static byte *Allocate(uint32 Size)
		{
			return Memory::Allocate(Size);
		}

		static void Deallocate(byte *Address)
		{
			Memory::Free(Address);
		}
	};
//}

#endif