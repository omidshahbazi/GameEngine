// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\Allocator\AllocatorBase.h>

#ifndef DEFAULT_ALLOCATOR_H
#define DEFAULT_ALLOCATOR_H

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class DefaultAllocator : public AllocatorBase
			{
			public:
				HandleInfo *Allocate(uint32 Size) override;

				void Deallocate(HandleInfo *Handle) override;
			};
		}
	}
}

#endif