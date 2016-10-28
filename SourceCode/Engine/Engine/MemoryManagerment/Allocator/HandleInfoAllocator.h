// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagerment\Allocator\AllocatorBase.h>

#ifndef HANDLE_INFO_ALLOCATOR_H
#define HANDLE_INFO_ALLOCATOR_H

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class HandleInfoAllocator : public AllocatorBase
			{
			public:
				HandleInfoAllocator(uint32 ReserveCount);

			public:
				HandleInfo *Allocate(void);

				void Deallocate(HandleInfo *Handle) override;

			private:
				uint32 m_ReserveCount;
				HandleInfo *m_FirstHandle;
				bool *m_HandlesStatus;
			};
		}
	}
}

#endif