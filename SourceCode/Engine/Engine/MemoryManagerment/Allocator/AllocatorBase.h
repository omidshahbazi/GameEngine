// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Common\PrimitiveTypes.h>
#include <MemoryManagerment\HandleInfo.h>

//using namespace Engine::Platform;

#ifndef ALLOCATOR_BASE_H
#define ALLOCATOR_BASE_H

//namespace Engine::MemoryManagement::Allocator
//{
	class AllocatorBase
	{
	public:
		virtual byte *Allocate(uint32 Size) = 0;

		virtual void Deallocate(byte *Address) = 0;
	};
//}

#endif