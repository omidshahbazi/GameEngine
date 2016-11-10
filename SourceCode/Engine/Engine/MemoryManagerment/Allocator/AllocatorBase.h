// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>

#ifndef ALLOCATOR_BASE_H
#define ALLOCATOR_BASE_H

namespace Engine
{
	using namespace Common;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			class MEMORYMANAGERMENT_API AllocatorBase
			{
			public:
				virtual byte *Allocate(uint64 Amount)
				{
					return nullptr;
				}

				virtual void Deallocate(byte *Address)
				{ }

			protected:
				void PlatformCopy(const byte *Source, byte *Destination, uint64 Size);
				void PlatformSet(byte *Address, int32 Value, uint64 Size);
			};
		}
	}
}

#endif