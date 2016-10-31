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
			class MemoryHandleAllocator : public AllocatorBase
			{
			public:
				MemoryHandleAllocator(uint32 ReserveCount);

			public:
				MemoryHandle *Allocate(void);

				void Deallocate(MemoryHandle *Handle) override;

			private:
				uint32 m_ReserveCount;
				byte *m_Memory;
				bool *m_HandlesStatus;
			};
		}
	}
}

#endif