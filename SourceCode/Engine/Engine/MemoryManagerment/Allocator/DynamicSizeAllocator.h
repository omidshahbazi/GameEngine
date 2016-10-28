// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagerment\Allocator\AllocatorBase.h>

#ifndef DYNAMIC_SIZE_ALLOCATOR_H
#define DYNAMIC_SIZE_ALLOCATOR_H

namespace Engine
{
	namespace MemoryManagement
	{
		namespace Allocator
		{
			class DynamicSizeAllocator : public AllocatorBase
			{
			public:
				DynamicSizeAllocator(uint32 ReserveSize);

			public:
				MemoryHandle *Allocate(uint32 Size);

				void Deallocate(MemoryHandle *Handle) override;

				void Update(void);

			private:
				MemoryHandle *GetFirstHandle(void) const;

			private:
				uint32 m_ReserveSize;
				MemoryHandle *m_LastHandleInfo;
			};
		}
	}
}

#endif