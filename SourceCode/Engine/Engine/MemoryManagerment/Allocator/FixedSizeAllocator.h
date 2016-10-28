// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagerment\Allocator\AllocatorBase.h>

#ifndef FIXED_SIZE_ALLOCATOR_H
#define FIXED_SIZE_ALLOCATOR_H

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class FixedSizeAllocator : public AllocatorBase
			{
			public:
				FixedSizeAllocator(uint32 BlockSize, uint32 BlockCount);

			public:
				HandleInfo *Allocate(void);

				void Deallocate(HandleInfo *Handle) override;

				void Update(void);

			private:
				uint32 m_BlockSize;
				uint32 m_BlockCount;
				HandleInfo *m_LastHandleInfo;
			};
		}
	}
}

#endif