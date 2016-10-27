// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Common\PrimitiveTypes.h>

#ifndef ALLOCATOR_BASE_H
#define ALLOCATOR_BASE_H

namespace Engine
{
	using namespace Common;

	namespace MemoryManagement
	{
		class HandleInfo;

		namespace Allocator
		{
			class AllocatorBase
			{
			public:
				virtual HandleInfo *Allocate(uint32 Size) = 0;

				virtual void Deallocate(HandleInfo *Address) = 0;

				virtual void Update(void)
				{
				}
			};
		}
	}
}

#endif