// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include "Debug.h"
#include <memory>

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

class DefaultAllocator
{
public:
	static byte *Allocate(uint32 Size)
	{
		byte *address = (byte*)malloc(Size);

		Assert(address != nullptr, "Allocation failed");

		return address;
	}

	static void Deallocate(byte *Address)
	{
		Assert(Address != nullptr, "Deallocation of nullptr is not applicable");

		free(Address);
	}
};

#endif